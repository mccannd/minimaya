#pragma once

#include "la.h"
#include "halfedge.h"
#include "vertex.h"
#include <QListWidgetItem>

#include <shaderprogram.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "myMath.h"

class HalfEdge;
class Vertex;
using namespace glm;
using namespace std;


class Face : public QListWidgetItem, public ShaderProgram::Drawable
{
private:
    int count;

    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
public:
    HalfEdge* start_edge; // points to arbitary starting HalfEdge of this Face
    vec4 color; // RGB1 of the face's color
    int id; // unique integer use to identify the face in menus and the like

    /**
     * BADLY MAINTAINED. ONLY UPDATED WITH SUBDIVISION IS CLICKED
     * WILL CHANGE TO WHENEVER POSITION IS CHANGED IF I HAVE TIME BRO.
     */
    Vertex *centroid;

    ///---------------
    /// Constructors
    /// --------------
    Face();
    Face(int i);
    Face(int i, vec4 c, HalfEdge* s);

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
};
