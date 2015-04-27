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
#include <scene/joint.h>
#include <la.h>
#include "skeletonparser.h"
#include "raytrace.h"


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
    ShaderProgram prog_skeleton;
    Mesh geom_mesh;


    Face* selected_face = NULL;
    HalfEdge* selected_edge = NULL;
    Vertex* selected_vertex = NULL;

    Camera camera;

    Raytrace *rt;

    std::vector<HalfEdge*> drawn_edges = {};
    std::vector<Joint*> skeleton_list = {};
public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    Mesh* getMesh();

    Joint* root_joint = NULL;
    Joint* selected_joint = NULL;
    bool skeleton_visible = true;
    // determines whether the mesh is rendered with lambert or skeleton
    bool skeleton_bound = false;

    // mesh interface functions
    void divideEdge();
    void triangulateFace();
    void recolorFace(float r, float g, float b);
    void moveVertex(float x, float y, float z);
    void deleteVertex();
    void resetMesh();
    void subdivideMesh(QListWidget* e, QListWidget* f, QListWidget* v);

    void selectNextEdge(QListWidget* qlw);
    void selectSymEdge(QListWidget *qlw);
    void selectJoint(QTreeWidgetItem* j);

    void addJoint();

    void drawSkeleton(Joint* j);
    void bindSkeleton();
    void resetSkeleton();

    void importOBJ();
    void importJSON();

    void updateSkeletonList();
    void updateSkeletonTransformations();

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void meshChanged();
    void vertexChosen(float x, float y, float z);
    void jointSelected(QTreeWidgetItem* j);
public slots:
    void faceSelected(QListWidgetItem* f);
    void edgeSelected(QListWidgetItem* e);
    void vertexSelected(QListWidgetItem* v);

};
