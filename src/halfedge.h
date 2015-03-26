#pragma once

#include <QListWidgetItem>
#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "vertex.h"
#include "face.h"

class Vertex;
class Face;
class HalfEdge : public QListWidgetItem, public ShaderProgram::Drawable
{
private:
    int id; // unique number for user interface

    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;
public:
    HalfEdge(int id);
    HalfEdge(Vertex* v, int id);
    HalfEdge(Face* f, int id);
    HalfEdge(Vertex* v, Face* f, int id);
    ~HalfEdge();

    // fields
    Face* face; // face to left of half edge
    Vertex* vert;
    HalfEdge* next;
    HalfEdge* sym;

    // pair this edge and another
    void pair (HalfEdge *s);
    int getID();

    // graphics: drawable line
    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};


