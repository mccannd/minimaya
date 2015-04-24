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
      //if (specular > 0.0f) diffuse += specular;
    }
    if (diffuse > 1.0f) diffuse = 1.0f;
    return (0.2f + diffuse) * trace.first->color;
  } else {
    return background;
  }
}

std::pair<Face*, Raytrace::OutgoingRays> Raytrace::traceRay(Raytrace::Ray r) {
  // Only traces with convex planar polygons
  float min = camera->far_clip;
  
  Face* closest = NULL;
  for (std::vector<Face*>::iterator it = mesh->faces.begin(); it != mesh->faces.end(); it++) {
    float t = r.intersect(*it);
    if (t < min && t > camera->near_clip) {
      min = t;
      closest = *it;
    }
  }
  glm::vec4 p = r.pos + t * r.dir;
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

std::pair<float, glm::vec4> Raytrace::Ray::intersect(Face *f) {
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
  return (inside) ? std::make_pair(t, p) : std::make_pair(FLT_MAX, glm::vec4(0, 0, 0, 1));
}





Octree::Octree(int depth, glm::vec3 lo, glm::vec3 hi) {
  this->depth = depth;
  this->lo = lo;
  this->hi = hi;
}

void Octree::split() {
  glm::vec3 mid = (lo + hi) / 2.0f;
  children[0] = new Octree(depth + 1, glm::vec3(lo[0],  lo[1],  lo[2] ), glm::vec3(mid[0], mid[1], mid[2]));
  children[1] = new Octree(depth + 1, glm::vec3(lo[0],  lo[1],  mid[2]), glm::vec3(mid[0], mid[1], hi[2] ));
  children[2] = new Octree(depth + 1, glm::vec3(mid[0], lo[1],  lo[2] ), glm::vec3(hi[0],  mid[1], mid[2]));
  children[3] = new Octree(depth + 1, glm::vec3(mid[0], lo[1],  mid[2]), glm::vec3(hi[0],  mid[1], hi[2] ));
  children[4] = new Octree(depth + 1, glm::vec3(lo[0],  mid[1], lo[2] ), glm::vec3(mid[0], hi[1],  mid[2]));
  children[5] = new Octree(depth + 1, glm::vec3(lo[0],  mid[1], mid[2]), glm::vec3(mid[0], hi[1],  hi[2] ));
  children[6] = new Octree(depth + 1, glm::vec3(mid[0], mid[1], lo[2] ), glm::vec3(hi[0],  hi[1],  mid[2]));
  children[7] = new Octree(depth + 1, glm::vec3(mid[0], mid[1], mid[2]), glm::vec3(hi[0],  hi[1],  hi[2] ));
}

void Octree::relocate(Face *f) {
  if (children[0] == NULL) return;
  std::pair<glm::vec3, glm::vec3> bounds = getBounds(f);
  if (bounded(children[0]->lo, children[0]->hi, bounds)) children[0]->add(f);
  if (bounded(children[1]->lo, children[1]->hi, bounds)) children[1]->add(f);
  if (bounded(children[2]->lo, children[2]->hi, bounds)) children[2]->add(f);
  if (bounded(children[3]->lo, children[3]->hi, bounds)) children[3]->add(f);
  if (bounded(children[4]->lo, children[4]->hi, bounds)) children[4]->add(f);
  if (bounded(children[5]->lo, children[5]->hi, bounds)) children[5]->add(f);
  if (bounded(children[6]->lo, children[6]->hi, bounds)) children[6]->add(f);
  if (bounded(children[7]->lo, children[7]->hi, bounds)) children[7]->add(f);
}

std::pair<glm::vec3, glm::vec3> Octree::getBounds(Face *f) {
  glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
  glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  HalfEdge* e = f->start_edge;
  do {
    glm::vec4 p = e->vert->pos;
    if (p[0] < min[0]) min[0] = p[0];
    if (p[1] < min[1]) min[1] = p[1];
    if (p[2] < min[2]) min[2] = p[2];
    if (p[0] > max[0]) max[0] = p[0];
    if (p[1] > max[1]) max[1] = p[1];
    if (p[2] > max[2]) max[2] = p[2];
    e = e->next;
  } while (e != f->start_edge);
  return std::make_pair(min, max);
}

bool Octree::bounded(glm::vec3 lo, glm::vec3 hi, std::pair<glm::vec3, glm::vec3> bounds) {
  if (bounds.second[0] < lo[0]) return false;
  if (bounds.second[1] < lo[1]) return false;
  if (bounds.second[2] < lo[2]) return false;
  if (bounds.first[0]  > hi[0]) return false;
  if (bounds.first[1]  > hi[1]) return false;
  if (bounds.first[2]  > hi[2]) return false;
  return true;
}

void Octree::add(Face *f) {
  if (faces.size() == 1 && depth < 4) {
    relocate(faces.front());
    faces.pop_back();
    relocate(f);
  } else {
    faces.push_back(f);
  }
}

bool Octree::intersect(Ray r) {
  return true; // TODO: ray cube intersection
}

std::pair<Face*, float> Octree::cast(Ray r) {
  if (faces.size() > 0) {
    if (intersect(r)) {
      float min = FLT_MAX;
      Face* closest = NULL;
      for (std::vector<Face*>::iterator it = faces.begin(); it != faces.end(); it++) {
        float t = r.intersect(*it);
        if (t < min) {
          min = t;
          closest = *it;
        }
      }
      return std::make_pair(closest, min);
    } else return std::make_pair(NULL, FLT_MAX);
  } else {
    float min = FLT_MAX;
    Face* closest = NULL;
    for (int i = 0; i < 8; i++) {
      std::pair<Face*, float> t = children[i]->cast(r);
      if (t.second < min) {
        min = t.second;
        closest = t.first;
      }
    }
    return std::make_pair(closest, min);
  }
}