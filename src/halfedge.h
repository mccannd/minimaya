#pragma once
#include "face.h"
#include "vertex.h"
#include <QListWidgetItem>

#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "myMath.h"

class Face;
class Vertex;
class mymath;

class HalfEdge : public QListWidgetItem, public ShaderProgram::Drawable
{
private:
    int count;

    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
public:
    Face *face; // points to the Face to the left of this HalfEdge
    Vertex *vert; // points to the vertex betwen this HalfEdge and next HalfEdge in the ring
    HalfEdge *next; // points to the next HalfEdge in the loop of Half Edges that make up
                    // this HalfEdge's Face
    HalfEdge *sym; //points to the HalfEdge that lies parallel to this HalfEdge and which travels
                   // in the opposite direction and is part of an adjacent face
    int id; // unique ID integer

    ///---------------
    /// Constructors
    /// --------------
    HalfEdge(int i);
    HalfEdge(int i, Vertex *v);
    HalfEdge(int i, Vertex *v, Face *f);
    HalfEdge(int i, Vertex *v, Face *f, HalfEdge *n, HalfEdge *s);

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};

