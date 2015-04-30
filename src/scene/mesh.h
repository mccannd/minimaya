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
    QOpenGLBuffer bufJID;   // when bound to a skeleton, these are pairs of influencing joint per vertex
    QOpenGLBuffer bufJWeight;   // shows weight of joint influence per vertex

    std::vector<glm::vec4> meshVertexPositions = {};
    std::vector<glm::vec4> meshVertexNormals = {};
    std::vector<glm::vec4> meshVertexColors = {};
    std::vector<GLuint> meshIndices = {};
    std::vector<glm::ivec2> meshJointIDs = {};
    std::vector<glm::vec2> meshJointWeights = {};

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
    virtual bool bindJID();
    virtual bool bindJWeight();

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
    Vertex* divideEdge(HalfEdge* e, bool updateBuffers);
    void triangulateFace(Face* f);
    void deleteVertex(Vertex* v);

    bool smoothNormals = true;

    void parseObj(QString& fileName);

    // binds a skeleton to each vertex
    void linearBinding(Joint* root);

    void subdivide(QListWidget* e, QListWidget* f, QListWidget* v);
};


