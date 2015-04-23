#include "raytrace.h"

#include <cmath>
#include <cfloat>

glm::vec4 Raytrace::background = glm::vec4(0.5, 0.5, 0.5, 1);

Raytrace::Raytrace(Camera *cam, Mesh *m) {
  camera = cam;
  mesh = m;
}

void Raytrace::renderToFile(QString filename) {
  Ray::camera = camera;
  Ray::init();
  out.SetSize(camera->width, camera->height);
  out.SetBitDepth(24);
  for (int x = 0; x < camera->width; x++) {
    for (int y = 0; y < camera->height; y++) {
      setPixel(x, y, castRay(x + 0.5, y + 0.5));
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
  std::pair<Face*, Ray> trace = traceRay(r);
  if (trace.first != NULL) {
    float diffuse = glm::dot(trace.first->norm, glm::normalize(glm::vec4(5, 5, 3, 1) - trace.second.pos));
    if (diffuse > 1.0f) diffuse = 1.0f;
    if (diffuse < 0.0f) diffuse = 0.0f;
    return (0.2f + diffuse) * trace.first->color;
  } else {
    return background;
  }
}

std::pair<Face*, Raytrace::Ray> Raytrace::traceRay(Raytrace::Ray r) {
  // Only traces with convex planar polygons
  float min = camera->far_clip;
  glm::vec4 p(0, 0, 0, 1);
  Face* closest = NULL;
  for (std::vector<Face*>::iterator it = mesh->faces.begin(); it != mesh->faces.end(); it++) {
    std::pair<float, glm::vec4> t = r.intersect(*it);
    if (t.first < min) {
      min = t.first;
      p = t.second;
      closest = *it;
    }
  }
  if (closest != NULL) {
    return std::make_pair(closest, Ray(p, glm::reflect(r.dir, closest->norm)));
  } else {
    return std::make_pair(closest, r);
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
  return (size(a, p, b) / s) + (size(b, p, c) / s) + (size(c, p, a) / s) == 1;
}

std::pair<float, glm::vec4> Raytrace::Ray::intersect(Face *f) {
  glm::vec4 a = f->start_edge->vert->pos;
  glm::vec4 b = f->start_edge->next->vert->pos;
  glm::vec4 c = f->start_edge->next->next->vert->pos;
  f->norm = glm::vec4(glm::normalize(glm::cross(glm::vec3(b - a), glm::vec3(c - a))), 0);
  float t = glm::dot(f->norm, a - pos) / glm::dot(f->norm, dir);
  glm::vec4 p = pos + t * dir;
  for (HalfEdge *e = f->start_edge->next; e->next != f->start_edge; e = e->next) {
    if (within(p, a, e->vert->pos, e->next->vert->pos)) return std::make_pair(t, p);
  }
  return std::make_pair(FLT_MAX, glm::vec4(0, 0, 0, 1));
}