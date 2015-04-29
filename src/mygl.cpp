
#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>



MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent)
{
    setFocusPolicy(Qt::ClickFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
    geom_cylinder.destroy();
    geom_sphere.destroy();
    // ensure that the data in the mesh does not leak
    geom_mesh.clearAll();
    geom_mesh.destroy();

    delete root_joint;
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    geom_cylinder.create();

    geom_sphere.create();

    geom_mesh.create();
    geom_mesh.unitCube(); // initialize mesh as a unit cube


    // create a root skellington joint
    root_joint = new Joint();
    root_joint->rename("Root");
    root_joint->create();

    selected_joint = root_joint;

    emit meshChanged();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the wireframe shader
    prog_wire.create(":/glsl/wire.vert.glsl", ":/glsl/wire.frag.glsl");

    prog_skeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/lambert.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();
}

void MyGL::resizeGL(int w, int h)
{
    camera = Camera(w, h);

    glm::mat4 viewproj = camera.getViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_wire.setViewProjMatrix(viewproj);
    prog_skeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the viewproj matrix
    prog_lambert.setViewProjMatrix(camera.getViewProj());
    prog_wire.setViewProjMatrix(camera.getViewProj());

    prog_lambert.setModelMatrix(glm::mat4(1.0f));
    prog_wire.setModelMatrix(glm::mat4(1.0f));

    prog_skeleton.setViewProjMatrix(camera.getViewProj());
    prog_skeleton.setModelMatrix(glm::mat4(1.0f));

    if (!skeleton_bound) {
        prog_lambert.draw(*this, geom_mesh);
    } else {
        prog_skeleton.draw(*this, geom_mesh);
    }

    // draw selected mesh features
    glDisable(GL_DEPTH_TEST);

    for (unsigned int i = 0; i < drawn_edges.size(); i++) {
        if (drawn_edges[i] != NULL) {
            drawn_edges[i]->create();
            prog_wire.draw(*this, *(drawn_edges[i]));
        }
    }

    if (selected_vertex != NULL) {
        selected_vertex->create();
        prog_wire.draw(*this, *selected_vertex);
    }

    if (skeleton_visible) {
        drawSkeleton(root_joint);
    }

    glEnable(GL_DEPTH_TEST);
}

// recursively draw all joints in the skeleton
void MyGL::drawSkeleton(Joint *j)
{
    if (j != NULL) {
        j->create();
        prog_wire.draw(*this, *j);
        for (unsigned int i = 0; i < j->numChildren(); i++) {
            drawSkeleton(j->getChild(i));
        }
    }
}

Mesh* MyGL::getMesh()
{
    return &geom_mesh;
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        camera.theta += 5.0f * DEG2RAD;
    } else if (e->key() == Qt::Key_Left) {
        camera.theta -= 5.0f * DEG2RAD;
    } else if (e->key() == Qt::Key_Up) {
        camera.phi -= 5.0f * DEG2RAD;
    } else if (e->key() == Qt::Key_Down) {
        camera.phi += 5.0f * DEG2RAD;
    } else if (e->key() == Qt::Key_I) {
        camera.zoom -= 0.5f;
    } else if (e->key() == Qt::Key_O) {
        camera.zoom += 0.5f;
    } else if (e->key() == Qt::Key_1) {
        camera.fovy += 5.0f * DEG2RAD;
    } else if (e->key() == Qt::Key_2) {
        camera.fovy -= 5.0f * DEG2RAD;
    }
    camera.RecomputeEye();
    update();  // Calls paintGL, among other things
}

/// mesh interface functions
void MyGL::divideEdge()
{
    // check that an edge is selected
    if (selected_edge == NULL) {
        return;
    }

    // tell the mesh to divide this edge
    geom_mesh.divideEdge(selected_edge, true);

    // emit a signal that the mesh has changed
    update();
    emit meshChanged();
}

void MyGL::recolorFace(float r, float g, float b)
{
    // check that a face is selected
    if (selected_face == NULL) {
        return;
    }

    if (r > 1) r = 1;
    if (r < 0) r = 0;
    if (g > 1) g = 1;
    if (g < 0) g = 0;
    if (b > 1) b = 1;
    if (b < 0) b = 0;

    geom_mesh.recolorFace(selected_face, r, g, b);
    //std::cout<<"FEHS COLOR\n";
    update();
}

void MyGL::triangulateFace()
{
    // check that a face is selected
    if (selected_face == NULL) {
        return;
    }

    // if the face is a triangle, do nothing
    if (drawn_edges.size() <= 3) {
        return;
    }

    // tell the mesh to divide this face
    geom_mesh.triangulateFace(selected_face);

    drawn_edges.clear();

    update();
    // emit a signal that the mesh has changed
    emit meshChanged();
}

void MyGL::moveVertex(float x, float y, float z)
{
    // check that a vertex is selected
    if (selected_vertex == NULL) {
        return;
    }

    // change the position of the selected vertex
    geom_mesh.moveVertex(selected_vertex, x, y, z);

    // update the screen
    update();
}

void MyGL::deleteVertex()
{
    // check that a vertex is selected
    if (selected_vertex == NULL) {
        return;
    }

    geom_mesh.deleteVertex(selected_vertex);

    selected_vertex = NULL;

    update();

    // emit a signal that the mesh has changed
    emit meshChanged();
}

void MyGL::subdivideMesh(QListWidget *e, QListWidget *f, QListWidget *v)
{
    geom_mesh.subdivide(e, f, v);
    update();
    selected_edge = NULL;
    selected_vertex = NULL;
    selected_face = NULL;
    drawn_edges.clear();
    emit meshChanged();
}

void MyGL::resetMesh()
{
    selected_face = NULL;
    selected_edge = NULL;
    selected_vertex = NULL;
    drawn_edges.clear();
    geom_mesh.clearAll();
    geom_mesh.unitCube();
    update();
    emit meshChanged();
    skeleton_bound = false;
}

void MyGL::resetSkeleton()
{
    selected_joint = NULL;
    delete root_joint;
    root_joint = new Joint();
    root_joint->rename("Root");
    update();
}

void MyGL::bindSkeleton()
{
    geom_mesh.linearBinding(root_joint);
}

void MyGL::importOBJ()
{
    // open the file
    QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"),
                                                    QString("/home"),
                                                    QString("Mesh Files (*.obj)"));
    geom_mesh.parseObj(fileName);
    update();
    emit meshChanged();
}

void MyGL::importJSON()
{
    // open the file
    QString fileName = QFileDialog::
            getOpenFileName(this, QString("Open File"),
                                  QString("/home"),
                                  QString("Skeleton JSON files (*.json)"));

    Joint* newRoot = parseSkeleton(fileName);
    if (newRoot != NULL) {
        selected_joint = NULL;
        delete root_joint;
        root_joint = newRoot;
        update();
    }
    selectJoint(root_joint);
}

///  geometry selection slots

void MyGL::selectNextEdge(QListWidget *qlw)
{
    if (selected_edge == NULL) {return;}
    selected_edge = selected_edge->next;
    drawn_edges.clear();
    drawn_edges.push_back(selected_edge);
    qlw->setCurrentItem(selected_edge);
    update();
}

void MyGL::selectSymEdge(QListWidget *qlw)
{
    if (selected_edge == NULL) {return;}
    selected_edge = selected_edge->sym;
    drawn_edges.clear();
    drawn_edges.push_back(selected_edge);
    qlw->setCurrentItem(selected_edge);
    update();
}


void MyGL::selectJoint(QTreeWidgetItem *j)
{
    Joint* joint = (Joint*) j;
    if (joint != NULL) {
        if(selected_joint != NULL){
            selected_joint->color = glm::vec4(0.1, 0.1, 0.6, 1);
        }
        selected_joint = joint;
        selected_joint->color = glm::vec4(1, 1, 1, 1);
        emit jointSelected(j);
        update();
    }
}

void MyGL::addJoint()
{
    Joint* j = new Joint(selected_joint);
    selectJoint(j);
}

void MyGL::faceSelected(QListWidgetItem* f)
{
    Face* face = (Face*) f;

    // clear the vector of pointers
    drawn_edges.clear();

    // add all of the edges around the face to the list of things to draw
    HalfEdge* edge = face->start_edge;

    do {
        drawn_edges.push_back(edge);
        edge->create(); // prepare the edge for drawing
        edge = edge->next;
    } while (edge != face->start_edge);

    //std::cout<<"A face has been selected!\n";

    selected_face = face;

    selected_edge = NULL;
    selected_vertex = NULL;
    // redraw
    update();
}

void MyGL::edgeSelected(QListWidgetItem *e)
{
    HalfEdge* edge = (HalfEdge*) e;

    // clear the vector of pointers
    drawn_edges.clear();

    //std::cout<<"An edge has been selected!\n";

    drawn_edges.push_back(edge);
    edge->create(); // prepare the edge for drawing

    selected_edge = edge;

    selected_vertex = NULL;
    selected_face = NULL;
    update();

}

void MyGL::vertexSelected(QListWidgetItem *v)
{
    Vertex* vertex = (Vertex*) v;

    drawn_edges.clear();

    selected_vertex = vertex;

    selected_edge = NULL;
    selected_face = NULL;

    emit vertexChosen(selected_vertex->pos[0],
            selected_vertex->pos[1],
            selected_vertex->pos[2]);

    update();
}

// helper: recursively produce a list of all joints
void jointsList(std::vector<Joint*> &joints, Joint* j)
{
    joints.push_back(j);
    for (unsigned int i = 0; i < j->numChildren(); i++) {
        jointsList(joints, j->getChild(i));
    }
}

void MyGL::updateSkeletonList()
{
    skeleton_list.clear();
    jointsList(skeleton_list, root_joint);

    std::vector<glm::mat4> bind;
    for (unsigned int i = 0; i < skeleton_list.size(); i++) {
        bind.push_back(skeleton_list[i]->bind_matrix);

    }

    prog_skeleton.setJointBindPosArray(bind);
}

void MyGL::updateSkeletonTransformations()
{
    std::vector<glm::mat4> trans;
    for (unsigned int i = 0; i < skeleton_list.size(); i++) {
        trans.push_back(skeleton_list[i]->getOverallTransformation());
        //std::cout << i << "t,";
    }
    //std::cout << "\n";
    prog_skeleton.setJointTransformArray(trans);
}

void MyGL::playAnimation(QListWidget *kf)
{

    // there must be at least two items to interpolate
    if (kf->count() < 3) {
        return;
    }

    timeline = kf;
    maxKeyframe = timeline->count() - 1;
    currentKeyframe = 0;

    if (!buttonConnected) {
        connect(&animationTimer, SIGNAL(timeout()),
            this, SLOT(updateAnimationFrame()));
        buttonConnected = true;
    }

    animationTimer.start(33);
}

void MyGL::updateAnimationFrame()
{

    root_joint->applyKeyframe(currentKeyframe);
    updateSkeletonTransformations();
    timeline->setCurrentRow(currentKeyframe);
    update();
    currentKeyframe++;
    if (currentKeyframe >= maxKeyframe) {
        animationTimer.stop();
        return;
    }
}

void MyGL::smoothNormals()
{
    geom_mesh.smoothNormals = !geom_mesh.smoothNormals;
    geom_mesh.create();
}
