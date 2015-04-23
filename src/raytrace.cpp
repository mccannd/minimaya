#include "raytrace.h"

#include <iostream>

#include <cstdlib>
#include <cmath>
#include <cfloat>

glm::vec4 Raytrace::background = glm::vec4(0, 0, 0, 1);
glm::vec4 Raytrace::light_source = glm::vec4(-1, 5, -1, 1);

Raytrace::Raytrace(Camera *cam, Mesh *m) {
  camera = cam;
  mesh = m;
}

void Raytrace::renderToFile(QString filename) {
  Ray::init(camera);
  out.SetSize(camera->width, camera->height);
  out.SetBitDepth(24);
  for (int x = 0; x < camera->width; x++) {
    for (int y = 0; y < camera->height; y++) {
      glm::vec4 samples;
      int n = 1;
      float incr = 1.0f / n;
      for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
          float jitter_x = rand() / (float) RAND_MAX;
          float jitter_y = rand() / (float) RAND_MAX;
          float sx = x + ((i + jitter_x) * incr);
          float sy = y + ((j + jitter_y) * incr);
          samples += castRay(sx, sy);
        }
      }
      setPixel(x, y, samples * incr * incr);
    }
  }
  out.WriteToFile(filename.toStdString().data());
}

void Raytrace::setPixel(int x, int y, glm::vec4 color) {
  glm::vec4 c = 255.0f * glm::abs(color);
  RGBApixel* pixel = out(x, y);
  pixel->Red   = c[0];
  pixel->Green = c[1];
  pixel->Blue  = c[2];
  pixel->Alpha = c[3];
}

glm::vec4 Raytrace::castRay(float x, float y) {
  Ray r(x, y);
  // Only traces with convex planar polygons
  std::pair<Face*, OutgoingRays> trace = traceRay(r);
  if (trace.first != NULL) {
    Ray to_light = std::get<2>(trace.second);
    float diffuse = glm::dot(trace.first->norm, glm::normalize(to_light.dir));
    if (diffuse < 0.0f) diffuse = 0.0f;
    else if (traceRay(to_light).first != NULL) diffuse = 0.0f; // shadow
    else {
      glm::vec4 H = (to_light.dir - to_light.pos + camera->eye) / 2.0f;
      float specular = std::pow(glm::dot(H, trace.first->norm), 12.8);
      if (specular > 0.0f) diffuse += specular;
    }
    if (diffuse > 2.0f) diffuse = 2.0f;
    return (0.2f + diffuse) * trace.first->color;
  } else {
    return background;
  }
}

std::pair<Face*, Raytrace::OutgoingRays> Raytrace::traceRay(Raytrace::Ray r) {
  // Only traces with convex planar polygons
  float min = camera->far_clip;
  glm::vec4 p(0, 0, 0, 1);
  Face* closest = NULL;
  for (std::vector<Face*>::iterator it = mesh->faces.begin(); it != mesh->faces.end(); it++) {
    std::pair<float, glm::vec4> t = r.intersect(*it);
    if (t.first < min && t.first > camera->near_clip) {
      min = t.first;
      p = t.second;
      closest = *it;
    }
  }
  if (closest != NULL) {
    return std::make_pair(closest, std::make_tuple(
      Ray(p, glm::reflect(r.dir, closest->norm)), 
      Ray(p, glm::refract(r.dir, closest->norm, 1.0f)), 
      Ray(p, light_source - p)));
  } else {
    return std::make_pair(closest, std::make_tuple(
      Ray(p, r.dir), 
      Ray(p, r.dir), 
      Ray(p, light_source - p)));
  }
}




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

float Raytrace::Ray::len = -1;
glm::vec4 Raytrace::Ray::V = glm::vec4();
glm::vec4 Raytrace::Ray::H = glm::vec4();
Camera* Raytrace::Ray::camera = NULL;

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

float Raytrace::Ray::size(glm::vec4 &a, glm::vec4 &b, glm::vec4 &c) {
  return glm::length(glm::cross(glm::vec3(b - a), glm::vec3(c - a))) / 2;
}

bool Raytrace::Ray::within(glm::vec4 &p, glm::vec4 &a, glm::vec4 &b, glm::vec4 &c) {
  float s = size(a, b, c);
  return fequal((size(a, p, b) / s) + (size(b, p, c) / s) + (size(c, p, a) / s), 1.0f);
}

std::pair<float, glm::vec4> Raytrace::Ray::intersect(Face *f) {
  glm::vec4 a = f->start_edge->vert->pos;
  glm::vec4 b = f->start_edge->next->vert->pos;
  glm::vec4 c = f->start_edge->next->next->vert->pos;
  if (f->norm[3] != 0) f->norm = glm::vec4(glm::normalize(glm::cross(glm::vec3(b - a), glm::vec3(c - a))), 0);
  float t = glm::dot(f->norm, a - pos) / glm::dot(f->norm, dir);
  glm::vec4 p = pos + t * dir;
  for (HalfEdge *e = f->start_edge->next; e->next != f->start_edge; e = e->next) {
    if (within(p, a, e->vert->pos, e->next->vert->pos)) return std::make_pair(t, p);
  }
  return std::make_pair(FLT_MAX, glm::vec4(0, 0, 0, 1));
}