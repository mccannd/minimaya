#pragma once

#include "la.h"
#include "halfedge.h"
#include <QListWidgetItem>
#include "scene/joint.h"

#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class HalfEdge;
class Face;
using namespace glm;

class Vertex : public QListWidgetItem, public ShaderProgram::Drawable
{
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol;
public:
    vec4 pos; // stores the position of a vertex
    HalfEdge* edge = NULL; // any HalfEdge that points to this Vertex
    int id; // unique integer used to identify the vertex in menus and the like

    ///---------------
    /// Constructors
    /// --------------
    Vertex(int i);
    Vertex(int i, vec4 p);
    Vertex(int i, vec4 p, HalfEdge* e);

    QPair<QPair<Joint*, float>, QPair<Joint*, float>> jointToweight = QPair<QPair<Joint*, float>, QPair<Joint*, float>>(
            QPair<Joint*, float>(NULL, std::numeric_limits<float>::max()), QPair<Joint*, float>(NULL, std::numeric_limits<float>::max()));

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};
