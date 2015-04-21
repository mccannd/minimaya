#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QFileDialog>

#include <glwidget277.h>
#include <la.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include <scene/camera.h>
#include <scene/mesh.h>
#include <scene/joint.h>
#include "halfedge.h"
#include "vertex.h"
#include "face.h"
#include <vector>
#include "mylistwidget.h"
#include <QSet>
#include "myMath.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <string>

using namespace glm;
using namespace std;

class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QOpenGLVertexArrayObject vao;

    Cylinder geom_cylinder;
    Sphere geom_sphere;
    Mesh geom_mesh;
    Joint* body;

    Face* f_selected = NULL;
    HalfEdge* h_selected = NULL;
    Vertex* v_selected = NULL;
    Joint* j_selected = NULL;

    ShaderProgram prog_lambert;
    ShaderProgram prog_wire;

    Camera camera;

    bool interaction = false;
    QString temp_vert[3] = {0, 0, 0};
    QString temp_col[3] = {0, 0, 0};

    int num = 0;

    void createJointGraph();
    void traverseJoint(Joint *j);

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    vector<Face*> createCube();
    void send_structures();
    void triangulate(Face* f);
    void quadrangulate(Face* f, vector<HalfEdge*> hes);
    void addMidpointtoEdge(HalfEdge* h);
    void Unreachable();
    void createJoints(Joint* j);
    Joint* JsonToJoint(QJsonObject qj, Joint* p);
    QPair<QPair<Joint*, float>, QPair<Joint*, float>> closestJoints(Vertex* v, Joint* j, QPair<QPair<Joint*, float>, QPair<Joint*, float>> res);

public slots:
    void slot_Triangulate();
    void slot_AddVertextoEdge();
    void slot_ReceiveSelectedHalfEdge(QListWidgetItem*);
    void slot_ReceiveSelectedVertex(QListWidgetItem*);
    void slot_ReceiveSelectedFace(QListWidgetItem*);
    void slot_ReceiveR(QString);
    void slot_ReceiveG(QString);
    void slot_ReceiveB(QString);
    void slot_Receivez(QString);
    void slot_Receivey(QString);
    void slot_Receivex(QString);
    void slot_UpdateVertexPos();
    void slot_UpdateFaceColor();
    void slot_DeleteVertex();
    void slot_Subdivide();
    void slot_OpenFile();
    void slot_ExportFile();
    void slot_OpenJSON();
    void slot_MeshSkinningFunction();
    void slot_ReceiveSelectedJoint(QTreeWidgetItem* j, QTreeWidgetItem* k);
    void slot_RotateXJoint();
    void slot_RotateYJoint();
    void slot_RotateZJoint();

signals:
    void sig_SendFace(QListWidgetItem*);
    void sig_SendVertices(QListWidgetItem*);
    void sig_SendHalfEdges(QListWidgetItem*);
    void sig_Sendx(double);
    void sig_Sendy(double);
    void sig_Sendz(double);
    void sig_SendColR(double);
    void sig_SendColG(double);
    void sig_SendColB(double);
    void sig_ResetList();
    void sig_ClearList();
    void sig_sendItemTop(QTreeWidgetItem*);
    void sig_sendJointTranslation(QString);
    void sig_sendJointQuat(QString);

protected:
    void keyPressEvent(QKeyEvent *e);
};
