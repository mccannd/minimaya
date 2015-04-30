#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTimer>

#include <glwidget277.h>
#include <la.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include <scene/camera.h>
#include <scene/mesh.h>
#include <scene/joint.h>
#include <scene/lattice.h>
#include <scene/latticeray.h>
#include <la.h>
#include "skeletonparser.h"
#include <scene/controls.h>
#include "raytrace.h"

class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QPoint prevPos;

    QOpenGLVertexArrayObject vao;

    Cylinder geom_cylinder;
    Sphere geom_sphere;
    ShaderProgram prog_lambert;
    ShaderProgram prog_wire;
    ShaderProgram prog_skeleton;
    Mesh geom_mesh;
    Lattice* geom_lattice;
    Controls* lattice_controls;

    Face* selected_face = NULL;
    HalfEdge* selected_edge = NULL;
    Vertex* selected_vertex = NULL;

    Camera camera;

    Raytrace *rt;

    std::vector<HalfEdge*> drawn_edges = {};
    std::vector<Joint*> skeleton_list = {};

    int currentKeyframe = 0;
    int maxKeyframe;

    QTimer animationTimer;
    QListWidget* timeline;
    bool buttonConnected = false;
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

    // Deformation Stuff
    bool lattice_active = false;
    bool lattice_show = false;
    bool onscreen_controls_active = false;
    int deformation_axis = 0; // Default X
    float deformation_intensity = 1; // Default Intensity
    int Lattice_Mode = 0; // Default Bend
    int Onscreen_Mode = 0; // Default Translate Mode

    // mesh interface functions
    void divideEdge();
    void triangulateFace();
    void recolorFace(float r, float g, float b);
    void materializeFace(float rflec, float rfrac, float alpha, float specl);
    void moveVertex(float x, float y, float z);
    void deleteVertex();
    void resetMesh();
    void subdivideMesh(QListWidget* e, QListWidget* f, QListWidget* v);
    void smoothNormals();

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
    void updateLightPosition(float x, float y, float z);
    void updateLightColor(float r, float g, float b);

    void playAnimation(QListWidget *kf);

    // Deformation
    LatticeRay* latticeRaycast(int x, int y);
    LatticeRay* lattice_ray = NULL;
    vector<Vertex*> selected_lattice_vertices = {};

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
signals:
    void meshChanged();
    void vertexChosen(float x, float y, float z);
    void jointSelected(QTreeWidgetItem* j);
public slots:
    void faceSelected(QListWidgetItem* f);
    void edgeSelected(QListWidgetItem* e);
    void vertexSelected(QListWidgetItem* v);
    void updateAnimationFrame();
    void slot_Lattice_Checkbox(bool b);
    void slot_Lattice_Intensity(int q);
    void slot_Lattice_Bend();
    void slot_Lattice_Squash();
    void slot_Lattice_Stretch();
    void slot_Lattice_Taper();
    void slot_Lattice_Twist();
    void slot_Lattice_X();
    void slot_Lattice_Y();
    void slot_Lattice_Z();
    void slot_Onscreen_Controls(bool b);
    void slot_Lattice_Show(bool b);
    void slot_Deformation_Commit();
    void slot_Onscreen_Translate();
    void slot_Onscreen_Rotate();
    void slot_Onscreen_Scale();
};
