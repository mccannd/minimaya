#pragma once

#include "QTreeWidgetItem"
#include "la.h"
#include <iostream>
#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

using namespace glm;
using namespace std;

class Joint : public ShaderProgram::Drawable, public QTreeWidgetItem
{
public:
    Joint();
    Joint(QString n, Joint* p, vec4 position, quat r);

    QString name; // displayed in your QTreeWidget of joints.
    Joint* parent;
    vector<Joint*> children = {};
    vec4 pos; // The position of this joint relative to its parent joint.
    quat rot; // The quaternion that represents this joint's current orientation.
    mat4 bindMatrix;

    bool selected = false;

    // concatenation of a joint's position and rotation
    mat4 getLocalTransformation();
    // concatentation of this joint's local transformation
    // with the transformations of its chain of parent joints.
    mat4 getOverallTransformation();

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();


private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol;  // Can be used to pass per-vertex color information to the shader, but is currently unused.

};
