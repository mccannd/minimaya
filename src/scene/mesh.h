#pragma once
#include <shaderprogram.h>
#include "la.h"
#include "halfedge.h"
#include "myMath.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

class Mesh : public ShaderProgram::Drawable
{
private:
    int count = 0;

    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
public:
    Mesh();
    Mesh(vector<Face*> f);

    vector<Face*> faces;

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};
