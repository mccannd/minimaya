#include "ray.h"

static const int RAY_IDX_COUNT = 2;
static const int RAY_VERT_COUNT = 2;

Ray::Ray()
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = vec4(0, 0, -1, 0); // Inwards
    ori = vec4(0, 0, 1, 1); // Origin
}

Ray::Ray(vec4 d)
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = normalize(d);
    ori = vec4(0, 0, 1, 1); // Origin
}

Ray::Ray(vec4 d, vec4 o)
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    std::cout << "in constructor";
    dir = normalize(d);
    ori = o;
}

///---------------------
/// Getters and Setters
/// --------------------

void Ray::setDirection(vec4 v1) {
    dir - normalize(v1);
}

vec4 Ray::getDirection() const {
    return dir;
}

/// ------------------------------------------
/// Ray Drawable Functions (Only in .cpp file)
/// ------------------------------------------

void Ray::create()
{
  GLuint ray_idx[RAY_IDX_COUNT];
  vec4 ray_vert_pos[RAY_VERT_COUNT];
  vec4 ray_vert_nor[RAY_VERT_COUNT];

  ray_idx[0] = 0;
  ray_idx[1] = 1;
  ray_vert_pos[0] = this->ori;
  ray_vert_pos[1] = this->ori + this->dir;

  count = RAY_IDX_COUNT;

  bufIdx.create();
  bufIdx.bind();
  bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufIdx.allocate(ray_idx, RAY_IDX_COUNT * sizeof(GLuint));

  bufPos.create();
  bufPos.bind();
  bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufPos.allocate(ray_vert_pos,RAY_VERT_COUNT * sizeof(vec4));

  bufNor.create();
  bufNor.bind();
  bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufNor.allocate(ray_vert_nor, RAY_VERT_COUNT * sizeof(vec4));
}

void Ray::destroy()
{
  bufIdx.destroy();
  bufPos.destroy();
  bufNor.destroy();
  bufCol.destroy();
}

GLenum Ray::drawMode()
{
  return GL_LINES;
}

int Ray::elemCount()
{
  return count;
}

bool Ray::bindIdx()
{
  return bufIdx.bind();
}

bool Ray::bindPos()
{
  return bufPos.bind();
}

bool Ray::bindNor()
{
  return bufNor.bind();
}

bool Ray::bindCol()
{
  return bufCol.bind();
}
