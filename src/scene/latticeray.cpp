#include "latticeray.h"
#include <iostream>

static const int LATTICERAY_IDX_COUNT = 2 ;
static const int LATTICERAY_VERT_COUNT = 2;

LatticeRay::LatticeRay()
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = vec4(0, 0, -1, 0); // Inwards
    ori = vec4(0, 0, 1, 1); // Origin
}

LatticeRay::LatticeRay(vec4 d, vec4 o)
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = normalize(d);
    ori = o;
}

///---------------------
/// Getters and Setters
/// --------------------

void LatticeRay::setDirection(vec4 v1) {
    dir - normalize(v1);
}

vec4 LatticeRay::getDirection() const {
    return dir;
}

float LatticeRay::latticeIntersect(mat4 m, Camera *c) {
    LatticeRay* r = this;

    // ray local origin
    vec4 rlo = la::inverse(m) * r->ori;
    // ray local direction
    vec4 rld = la::inverse(m) * r->dir;

    float A = rld[0] * rld[0] + rld[1] * rld[1] + rld[2] * rld[2];
    float B = 2 * (rld[0] * rlo[0] + rld[1] * rlo[1] + rld[2] * rlo[2]);
    float C = rlo[0] * rlo[0] + rlo[1] * rlo[1] + rlo[2] * rlo[2] - 0.005f * 0.005f;

    float det = B * B - 4 * A * C;
    if (det < 0) {
        return -1;
    }

    float t_0 = ((-1 * B) - la::sqrt(det)) / 2 * A;
    float t_1 = ((-1 * B) + la::sqrt(det)) / 2 * A;

    // Return back to world_t
    if (t_0 > 0) {
        vec4 Pw = m * (rlo + t_0 * rld);
        return distance(Pw, c->eye);
    }
     else if (t_1 > 0){
        vec4 Pw = m * (rlo + t_1 * rld);
        return distance(Pw, c->eye);
    } else {
        return -1.0;
    }
}




/// ------------------------------------------
/// LatticeRay Drawable Functions (Only in .ccp file)
/// ------------------------------------------

void LatticeRay::create()
{
  GLuint LatticeRay_idx[LATTICERAY_IDX_COUNT];
  vec4 LatticeRay_vert_pos[LATTICERAY_VERT_COUNT];
  vec4 LatticeRay_vert_col[LATTICERAY_VERT_COUNT];

  LatticeRay_idx[0] = 0;
  LatticeRay_idx[1] = 1;
  LatticeRay_vert_pos[0] = this->ori;
  LatticeRay_vert_pos[1] = this->ori + this->dir;
  LatticeRay_vert_col[0] = vec4(1, 0, 0, 1);
  LatticeRay_vert_col[1] = vec4(0, 1, 0, 1);

  count = LATTICERAY_IDX_COUNT;

  bufIdx.create();
  bufIdx.bind();
  bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufIdx.allocate(LatticeRay_idx, LATTICERAY_IDX_COUNT * sizeof(GLuint));

  bufPos.create();
  bufPos.bind();
  bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufPos.allocate(LatticeRay_vert_pos, LATTICERAY_VERT_COUNT * sizeof(vec4));

  bufCol.create();
  bufCol.bind();
  bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufCol.allocate(LatticeRay_vert_col, LATTICERAY_VERT_COUNT * sizeof(vec4));
}

void LatticeRay::destroy()
{
  bufIdx.destroy();
  bufPos.destroy();
  bufNor.destroy();
  bufCol.destroy();
}

GLenum LatticeRay::drawMode()
{
  return GL_LINES;
}

int LatticeRay::elemCount()
{
  return count;
}

bool LatticeRay::bindIdx()
{
  return bufIdx.bind();
}

bool LatticeRay::bindPos()
{
  return bufPos.bind();
}

bool LatticeRay::bindNor()
{
  return bufNor.bind();
}

bool LatticeRay::bindCol()
{
  return bufCol.bind();
}

bool LatticeRay::bindJID()
{
    return 1;
}

bool LatticeRay::bindJWeight()
{
    return 1;
}
