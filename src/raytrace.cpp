#include "raytrace.h"

#include <iostream>

#include <cstdlib>
#include <cmath>
#include <cfloat>

#include <QThreadPool>

glm::vec4 Raytrace::background = glm::vec4(0, 0, 0, 1);

glm::vec4 Raytrace::light_source = glm::vec4(5, 5, 3, 1);

Raytrace::Raytrace(Camera *cam, Mesh *m) {
  camera = cam;
  mesh = m;
  Ray::init(camera);
}

void Raytrace::renderToFile(QString filename) {
  Ray::init();
  out.SetSize(camera->width, camera->height);
  out.SetBitDepth(24);
  Raytrace::Pixel::RT = this;
  for (int x = 0; x < camera->width; x++) {
    for (int y = 0; y < camera->height; y++) {
      Pixel *pix = new Pixel(out(x,y), x, y);
      QThreadPool::globalInstance()->start(pix);
    }
  }
  QThreadPool::globalInstance()->waitForDone();
  out.WriteToFile(filename.toStdString().data());
}





Raytrace* Raytrace::Pixel::RT = NULL;

Raytrace::Pixel::Pixel(RGBApixel* out, int x, int y) {
  this->out = out;
  this->x = x;
  this->y = y;
}

void Raytrace::Pixel::run() {
  glm::vec4 samples;
  float incr = 1.0f / SAMPLES;
  for (int i = 0; i < SAMPLES; i++) {
    for (int j = 0; j < SAMPLES; j++) {
      float jitter_x = rand() / (float) RAND_MAX;
      float jitter_y = rand() / (float) RAND_MAX;
      float sx = x + ((i + jitter_x) * incr);
      float sy = y + ((j + jitter_y) * incr);
      Ray r(sx, sy);
      samples += castRay(r, 5);
    }
  }
  glm::vec4 c = 255.0f * glm::abs(samples * incr * incr);
  out->Red   = c[0];
  out->Green = c[1];
  out->Blue  = c[2];
  out->Alpha = c[3];
}


glm::vec4 Raytrace::Pixel::castRay(Raytrace::Ray r, int depth) {
  if (depth < 1) return background;
  // Only traces with convex planar polygons
  std::pair<Face*, OutgoingRays> trace = traceRay(r);
  

  if (trace.first != NULL) {
    Material mat = trace.first->mat_attr;

    glm::vec4 final;
    float light = 0.0f;

    Ray to_light = std::get<2>(trace.second);
    float diffuse = glm::dot(trace.first->norm, glm::normalize(to_light.dir));
    light += (diffuse > 1.0f) ? 1.0f : (diffuse < 0.0f) ? 0.0f : diffuse;

    glm::vec4 H = glm::normalize((to_light.dir - to_light.pos + RT->camera->eye) / 2.0f);
    float specular = std::pow(glm::dot(H, trace.first->norm), mat.specl);
    light += (specular > 1.0f) ? 1.0f : (specular < 0.0f) ? 0.0f : specular;

    if (traceRay(to_light).first != NULL) light = 0.0f;

    final += mat.alpha * (light + 0.2f) * trace.first->color;

    if (mat.alpha < 1.0f) final += (1 - mat.alpha) * castRay(std::get<1>(trace.second), depth - 1);

    if (mat.rflec > 0.0f) final = (1 - mat.rflec) * final + (mat.rflec * castRay(std::get<0>(trace.second), depth - 1));

    return final;
  } else return background;
}

std::pair<Face*, Raytrace::OutgoingRays> Raytrace::Pixel::traceRay(Raytrace::Ray r) {
  // Only traces with convex planar polygons
  float min = RT->camera->far_clip;
  Face* closest = NULL;

  for (std::vector<Face*>::iterator it = RT->mesh->faces.begin(); it != RT->mesh->faces.end(); it++) {
    float t = r.intersect(*it);
    if (t < min && t > 0.0001) {
      min = t;
      closest = *it;
    }
  }

  glm::vec4 p = r.pos + min * r.dir;
  if (closest != NULL) {
    return std::make_pair(closest, std::make_tuple(
      Ray(p, glm::reflect(r.dir, closest->norm)), 
      Ray(p, glm::refract(r.dir, closest->norm, closest->mat_attr.rfrac)), 
      Ray(p, light_source - p)));
  } else {
    return std::make_pair(closest, std::make_tuple(
      Ray(p, r.dir), 
      Ray(p, r.dir), 
      Ray(p, light_source - p)));
  }
}





float Raytrace::Ray::len = -1;

glm::vec4 Raytrace::Ray::V = glm::vec4();

glm::vec4 Raytrace::Ray::H = glm::vec4();

Camera* Raytrace::Ray::camera = NULL;

Raytrace::Ray::Ray(float px, float py) {
  if (camera == NULL) return;
  float sx = (2 * px / camera->width) - 1;
  float sy = 1 - (2 * py / camera->height);
  pos = camera->eye;
  dir = camera->ref - camera->eye + (sx * H) + (sy * V);
}

Raytrace::Ray::Ray(glm::vec4 pos, glm::vec4 dir) {
  this->pos = pos;
  this->dir = dir;
}

void Raytrace::Ray::init(Camera *cam) {
  camera = cam;
  init();
}

void Raytrace::Ray::init() {
  if (camera == NULL) return;
  len = glm::distance(camera->ref, camera->eye) * tan(camera->fovy / 2);
  float aspect = camera->width / camera->height;
  V = len * camera->up;
  H = len * aspect * glm::vec4(glm::cross(glm::vec3(glm::normalize(camera->ref - camera->eye)), glm::vec3(camera->up)), 0);
}

bool Raytrace::Ray::within(glm::vec4 &norm, glm::vec4 &p, glm::vec4 &a, glm::vec4 &b) {
  return glm::dot(glm::vec3(norm), glm::cross(glm::vec3(p - a), glm::vec3(b - a))) < 0.0f;
}

float Raytrace::Ray::intersect(Face *f) {
  glm::vec4 a = f->start_edge->vert->pos;
  glm::vec4 b = f->start_edge->next->vert->pos;
  glm::vec4 c = f->start_edge->next->next->vert->pos;
  if (f->norm[3] != 0) f->norm = glm::vec4(glm::normalize(glm::cross(glm::vec3(b - a), glm::vec3(c - a))), 0);
  float t = glm::dot(f->norm, a - pos) / glm::dot(f->norm, dir);
  glm::vec4 p = pos + t * dir;
  bool inside = true;
  HalfEdge *e = f->start_edge;
  do {
    inside &= within(f->norm, p, e->vert->pos, e->next->vert->pos);
    e = e->next;
  } while (e != f->start_edge);
  return (inside) ? t : FLT_MAX;
}


