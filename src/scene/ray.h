#pragma once

#include <shaderprogram.h>
#include <la.h>
#include <iostream>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

using namespace glm;

class Ray : public ShaderProgram::Drawable
{
 private:
  int count;
  QOpenGLBuffer bufIdx;
  QOpenGLBuffer bufPos;
  QOpenGLBuffer bufNor;
  QOpenGLBuffer bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.

 public:
  Ray();
  Ray(vec4 d);
  Ray(vec4 o, vec4 d);

  vec4 ori;
  vec4 dir;

  void setDirection(vec4 v1);
  vec4 getDirection() const;

  void create();
  void destroy();

  virtual GLenum drawMode();
  virtual int elemCount();
  virtual bool bindIdx();
  virtual bool bindPos();
  virtual bool bindNor();
  virtual bool bindCol();
};
