#pragma once

#include <vector>
#include <map>
#include <utility>

#include <la.h>

#include <scene/camera.h>
#include <scene/mesh.h>

#include <bmp/EasyBMP.h>

class Raytrace {
  public:
    class Ray;
    static glm::vec4 background;

    Raytrace(Camera *cam, Mesh *m);

    void renderToFile(QString filename);

  private:
    Camera *camera;
    Mesh *mesh;

    BMP out;

    void setPixel(int x, int y, glm::vec4 color);
    glm::vec4 castRay(float x, float y);
    std::pair<Face*, Raytrace::Ray> traceRay(Raytrace::Ray r);
};

class Raytrace::Ray {
  private:
    static float len;
    static glm::vec4 V, H;

  public:
    static Camera *camera;
    static void init();

    static float size(glm::vec4 &a, glm::vec4 &b, glm::vec4 &c);

    glm::vec4 pos;
    glm::vec4 dir;

    Ray(float px, float py);
    Ray(glm::vec4 pos, glm::vec4 dir);

    std::pair<float, glm::vec4> intersect(Face *f);
    bool within(glm::vec4 &p, glm::vec4 &a, glm::vec4 &b, glm::vec4 &c);
};