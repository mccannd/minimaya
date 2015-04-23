#pragma once

#include <la.h>
#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <iostream>
#include <scene/mesh.h>
#include "mymath.h"

using namespace std;
using namespace glm;

class Lattice : public ShaderProgram::Drawable {
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufJID;   // when bound to a skeleton, these are pairs of influencing joint per vertex
    QOpenGLBuffer bufJWeight;   // shows weight of joint influence per vertex
public:
//    Lattice();
    Lattice(Mesh* m, int a, int b, int c);

    Mesh* mesh;
    int x; //
    int y;
    int z;

    float maxx = -INFINITY;
    float minx = INFINITY;
    float maxy = -INFINITY;
    float miny = INFINITY;
    float maxz = -INFINITY;
    float minz = INFINITY;

    void boundaries(Mesh *m);

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
