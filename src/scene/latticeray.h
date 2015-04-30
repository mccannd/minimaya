#pragma once

#include "lattice.h"
#include <shaderprogram.h>
#include <la.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class LatticeRay : public ShaderProgram::Drawable
{
 private:
  int count;
  QOpenGLBuffer bufIdx;
  QOpenGLBuffer bufPos;
  QOpenGLBuffer bufNor;
  QOpenGLBuffer bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
  QOpenGLBuffer bufJID;   // when bound to a skeleton, these are pairs of influencing joint per vertex
  QOpenGLBuffer bufJWeight;   // shows weight of joint influence per vertex

 public:
  LatticeRay();
  LatticeRay(vec4 o, vec4 d);

  vec4 ori;
  vec4 dir;

  void setDirection(vec4 v1);
  vec4 getDirection() const;

  float latticeIntersect(mat4 m, Camera *c);
  float onscreenInterset(mat4 m, Camera *c);

  void create();
  void destroy();

  virtual GLenum drawMode();
  virtual int elemCount();
  virtual bool bindIdx();
  virtual bool bindPos();
  virtual bool bindNor();
  virtual bool bindCol();
  virtual bool bindJID();
  virtual bool bindJWeight();
};
