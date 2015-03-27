#pragma once

#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "vertex.h"
#include "halfedge.h"

class Mesh : public ShaderProgram::Drawable
{
private:
    int count;
    int vertexID;
    int faceID;
    int edgeID;

    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufNor;
    QOpenGLBuffer bufCol;  // Can be used to pass per-vertex color information to the shader, but is currently unused.

    std::vector<glm::vec4> meshVertexPositions = {};
    std::vector<glm::vec4> meshVertexNormals = {};
    std::vector<glm::vec4> meshVertexColors = {};
    std::vector<GLuint> meshIndices = {};

    // clear the current buffers and repopulate
    void updateBuffers();


public:
    Mesh();

    void create();
    void destroy();

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();

    std::vector<HalfEdge*> edges = {};
    std::vector<Vertex*> vertices = {};
    std::vector<Face*> faces = {};

    // remake mesh using face traversals
    void arbitraryMesh(std::vector<Face*> f,
                       std::vector<Vertex*> v,
                       std::vector<HalfEdge*> e);
    // transforms this instance into a unit cube
    void unitCube();
    // destroy all information (edges, vertices, faces)
    void clearAll();

    /// Interface interaction options
    void moveVertex(Vertex* v, float x, float y, float z);
    void recolorFace(Face* f, float r, float g, float b);
    void divideEdge(HalfEdge* e);
    void triangulateFace(Face* f);
    void deleteVertex(Vertex* v);
};


