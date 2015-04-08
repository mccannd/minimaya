#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <glwidget277.h>
#include <la.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include <scene/camera.h>
#include <scene/mesh.h>
#include <la.h>


class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QOpenGLVertexArrayObject vao;

    Cylinder geom_cylinder;
    Sphere geom_sphere;
    ShaderProgram prog_lambert;
    ShaderProgram prog_wire;
    Mesh geom_mesh;

    Face* selected_face = NULL;
    HalfEdge* selected_edge = NULL;
    Vertex* selected_vertex = NULL;

    Camera camera;

    std::vector<HalfEdge*> drawn_edges = {};
public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    Mesh* getMesh();

    // mesh interface functions
    void divideEdge();
    void triangulateFace();
    void recolorFace(float r, float g, float b);
    void moveVertex(float x, float y, float z);
    void deleteVertex();
    void resetMesh();
    void subdivideMesh();

    void selectNextEdge(QListWidget* qlw);
    void selectSymEdge(QListWidget *qlw);

    void importOBJ();

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void meshChanged();
    void vertexChosen(float x, float y, float z);
public slots:
    void faceSelected(QListWidgetItem* f);
    void edgeSelected(QListWidgetItem* e);
    void vertexSelected(QListWidgetItem* v);

};
