#pragma once

#include <QListWidgetItem>
#include "halfedge.h"
#include "la.h"

#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "scene/joint.h"

class HalfEdge;
class Vertex : public QListWidgetItem,
        public ShaderProgram::Drawable
{
private:
    int id; // a unique id for user interface purposes
    int count = 1;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;
public:
    // constructors
    Vertex(int id);
    Vertex(float x, float y, float z, int id);
    ~Vertex();

    glm::vec4 pos; // location of vertex
    HalfEdge* edge = NULL;
    glm::ivec2 jointIDs;
    glm::vec2 weights;

    int getID();
    // graphics: drawable point
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

