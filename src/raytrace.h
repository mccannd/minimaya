#pragma once

#include <tuple>
#include <map>
#include <utility>
#include <vector>

#include <la.h>

#include <scene/camera.h>
#include <scene/mesh.h>

#include <bmp/EasyBMP.h>
#include <QRunnable>

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

    typedef std::tuple<Raytrace::Ray, Raytrace::Ray, Raytrace::Ray> OutgoingRays;

    class Pixel : public QRunnable {
      public:
        static const int SAMPLES = 2;
        static Raytrace *RT;

        RGBApixel *out;
        int x, y;

        Pixel(RGBApixel* out, int x, int y);

        void run();

      private:
        glm::vec4 castRay(Raytrace::Ray r, int depth);
        std::pair<Face*, Raytrace::OutgoingRays> traceRay(Raytrace::Ray r);
        float intersectSphere(Raytrace::Ray r);
    };

    

    static glm::vec4 background;
    static glm::vec4 light_source;
    static Face *sphere;
    static const bool perfect_sphere = false;

    Raytrace(Camera *cam, Mesh *m);

    void renderToFile(QString filename);

  private:
    Camera *camera;
    Mesh *mesh;

    BMP out;
};

