#pragma once

#pragma once

#include <la.h>
#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <iostream>
#include <scene/mesh.h>
#include "mymath.h"
#include <math.h>
#include "camera.h"

using namespace std;
using namespace glm;

class Controls : public ShaderProgram::Drawable {
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufJID;   // when bound to a skeleton, these are pairs of influencing joint per vertex
    QOpenGLBuffer bufJWeight;   // shows weight of joint influence per vertex

public:
    Controls();
    Controls(vec4 o, vec4 d);

    vec4 ori;
    vec4 dir;
    int TRS;

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
    virtual bool bindJID();
    virtual bool bindJWeight();

};
