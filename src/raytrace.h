#pragma once

#include <tuple>
#include <map>
#include <utility>
#include <vector>

#include <la.h>

#include <scene/camera.h>
#include <scene/mesh.h>

#include <bmp/EasyBMP.h>

class Raytrace {
  public:
    class Ray {
      private:
        static float len;
        static glm::vec4 V, H;

      public:
        static Camera *camera;
        static void init(Camera *cam);
        static void init();

        glm::vec4 pos;
        glm::vec4 dir;

        Ray(float px, float py);
        Ray(glm::vec4 pos, glm::vec4 dir);

        float intersect(Face *f);
        bool within(glm::vec4 &p, glm::vec4 &a, glm::vec4 &b, glm::vec4 &c);
    };

    class Octree {
      private:
        glm::vec3 lo, hi;
        int depth;

        Octree* children[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
        std::vector<Face*> faces = {};

        void split();
        void relocate(Face *f);

        static std::pair<glm::vec3, glm::vec3> getBounds(Face *f);
        static bool bounded(glm::vec3 lo, glm::vec3 hi, std::pair<glm::vec3, glm::vec3> bounds);

      public:
        Octree(int depth, glm::vec3 lo_bound, glm::vec3 hi_bound);

        void add(Face *f);
        std::pair<Face*, float> cast(Ray r);
        bool intersect(Ray r);
    };

    typedef std::tuple<Raytrace::Ray, Raytrace::Ray, Raytrace::Ray> OutgoingRays;

    static glm::vec4 background;
    static glm::vec4 light_source;

    Raytrace(Camera *cam, Mesh *m);

    void renderToFile(QString filename);

  private:
    Camera *camera;
    Mesh *mesh;
    Octree *octree;

    BMP out;

    void setPixel(int x, int y, glm::vec4 color);
    glm::vec4 castRay(float x, float y);
    std::pair<Face*, Raytrace::OutgoingRays> traceRay(Raytrace::Ray r);
};

