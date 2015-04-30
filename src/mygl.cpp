
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
    geom_lattice->destroy();
    lattice_controls->destroy();
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
    //geom_mesh.unitCube(); // initialize mesh as a unit cube

    QString filename = QString("C:/Users/molisani/stage.obj");
    geom_mesh.parseObj(filename);

    geom_lattice = new Lattice(&geom_mesh);
    geom_lattice->create();
    lattice_controls = new Controls();
    lattice_controls->create();


    


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

    // set the initial light position and color
    updateLightPosition(5, 5, 3);
    updateLightColor(1, 1, 1);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();
}

void MyGL::resizeGL(int w, int h)
{
    camera = Camera(w, h);
    rt = new Raytrace(&camera, &geom_mesh);

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

    if (skeleton_visible && !lattice_active) {
        drawSkeleton(root_joint);
    }

    if (lattice_show && lattice_active) {
        prog_wire.draw(*this, *geom_lattice);

        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->create();
            prog_wire.draw(*this, *selected_lattice_vertices[i]);
        }

//        if (lattice_ray != NULL) {
//            mat4 model = mat4(1.0f);
//            prog_wire.setModelMatrix(model);
//            lattice_ray->create();
//            prog_wire.draw(*this, *lattice_ray);
//        }
    }

    if (onscreen_controls_active) {
        prog_wire.draw(*this, *lattice_controls);
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
    } else if (e->key() == Qt::Key_W) {
        geom_lattice->updateDivisions(geom_lattice->x,
                                      geom_lattice->y + 1,
                                      geom_lattice->z);
    } else if (e->key() == Qt::Key_A) {
        geom_lattice->updateDivisions(geom_lattice->x - 1,
                                      geom_lattice->y,
                                      geom_lattice->z);
    } else if (e->key() == Qt::Key_S) {
        geom_lattice->updateDivisions(geom_lattice->x,
                                      geom_lattice->y - 1,
                                      geom_lattice->z);
    } else if (e->key() == Qt::Key_D) {
        geom_lattice->updateDivisions(geom_lattice->x + 1,
                                      geom_lattice->y,
                                      geom_lattice->z);
    } else if (e->key() == Qt::Key_E) {
        geom_lattice->updateDivisions(geom_lattice->x,
                                      geom_lattice->y,
                                      geom_lattice->z + 1);
    } else if (e->key() == Qt::Key_Q) {
        geom_lattice->updateDivisions(geom_lattice->x,
                                      geom_lattice->y,
                                      geom_lattice->z - 1);
    } else if (e->key() == Qt::Key_U) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(0, 1, 0, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_J) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(0, -1, 0, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_K) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(1, 0, 0, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_H) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(-1, 0, 0, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_Y) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(0, 0, 1, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_G) {
        for(std::vector<Vertex*>::size_type i = 0; i < selected_lattice_vertices.size(); i++) {
            selected_lattice_vertices[i]->pos += vec4(0, 0, -1, 0);
        }

        geom_lattice->freeFormDeformation();
        geom_lattice->create();
        geom_mesh.create();
        update();
        emit meshChanged();
    } else if (e->key() == Qt::Key_R) {
        rt->renderToFile("render.bmp");
    } else if (e->key() == Qt::Key_M) {
        geom_lattice->recreateLattice();
    }

    camera.RecomputeEye();
    update();  // Calls paintGL, among other things
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    prevPos = e->pos();
}


// For lattice
void MyGL::mouseReleaseEvent(QMouseEvent *e) {
    if (lattice_active && lattice_show) {
        prevPos = e->pos();
        lattice_ray = latticeRaycast(e->x(), e->y());

        std::vector<Vertex*> ray_pierced = {};
        std::vector<float> world_t = {};

        if(!QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier))
        {
            selected_lattice_vertices = {};
        }

        for(std::vector<Vertex*>::size_type i = 0; i < geom_lattice->ctrlpts.size(); i++) {
            mat4 m = translate(mat4(1.0f), vec3(geom_lattice->ctrlpts[i]->pos));
            float t = lattice_ray->latticeIntersect(m, &camera);
            if (t > 0) {
                ray_pierced.push_back(geom_lattice->ctrlpts[i]);
                world_t.push_back(t);
            }
        }

        if (!ray_pierced.empty()) {
            Vertex* closest_t = ray_pierced[0];
            float closest_world_t = world_t[0];
            for(std::vector<Vertex*>::size_type i = 1; i < ray_pierced.size(); i++) {
                if (world_t[i] < closest_world_t) {
                    closest_t = ray_pierced[i];
                    closest_world_t = world_t[i];
                }
            }
            selected_lattice_vertices.push_back(closest_t);
        }
        update();
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
//     int dx = e->x() - prevPos.x();
//     int dy = e->y() - prevPos.y();

//     if (e->buttons() & Qt::LeftButton) {
//         camera.theta += dx * 0.0002;
//         camera.phi += dy * 0.0002;
//     }

//     camera.RecomputeEye();
//     update();
}

/// Raycasting
LatticeRay* MyGL::latticeRaycast(int x, int y) {
    float sx = (2 * x/camera.width) - 1;
    float sy = 1 - (2 * y/camera.height);

    vec3 F = normalize(vec3(camera.ref) - vec3(camera.eye));
    vec3 R = normalize(cross(F, vec3(camera.up)));
    vec3 U = normalize(cross(R, F));

    float length = distance(camera.ref, camera.eye) * tan(camera.fovy/2);
    vec3 V = U * length;
    vec3 H = length * camera.width/camera.height * R;
    vec3 p = vec3(camera.ref) + sx * H + sy * V;

    vec4 ray_origin = camera.eye;
    vec4 ray_direction = normalize(vec4(p[0], p[1], p[2], 1) - camera.eye);

    return new LatticeRay(ray_origin, ray_direction);
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


void MyGL::materializeFace(float rflec, float rfrac, float alpha, float specl)
{
    // check that a face is selected
    if (selected_face == NULL) {
        return;
    }

    selected_face->mat_attr = Material(rflec, rfrac, alpha, specl);
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
    if (lattice_active) {
        geom_lattice->recreateLattice();
    }
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
    geom_lattice->recreateLattice();
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

void MyGL::updateLightPosition(float x, float y, float z)
{
    prog_lambert.setLightPosition(glm::vec4(x, y, z, 1));
    prog_skeleton.setLightPosition(glm::vec4(x, y, z, 1));
}

void MyGL::updateLightColor(float r, float g, float b)
{
    prog_lambert.setLightColor(glm::vec4(r, g, b, 1));
    prog_skeleton.setLightColor(glm::vec4(r, g, b, 1));
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

void MyGL::slot_Lattice_Checkbox(bool b) {
    lattice_active = b;
}

void MyGL::slot_Lattice_Intensity(int q) {
    deformation_intensity = q;

    if (lattice_active) {
        switch(Lattice_Mode) {
            case 0: // Bend
                geom_lattice->bending(deformation_intensity * DEG2RAD, deformation_axis);
                geom_lattice->create();
                geom_mesh.create();
                update();
                emit meshChanged();
                break;
            case 1: // Squash
                geom_lattice->squashing(deformation_intensity, deformation_axis);
                geom_lattice->create();
                geom_mesh.create();
                update();
                emit meshChanged();
                break;
            case 2: // Stretch
                geom_lattice->stretching(deformation_intensity, deformation_axis);
                geom_lattice->create();
                geom_mesh.create();
                update();
                emit meshChanged();
                break;
            case 3: // Taper
                geom_lattice->tapering(deformation_intensity * DEG2RAD / 4.15f, deformation_axis);
                geom_lattice->create();
                geom_mesh.create();
                update();
                emit meshChanged();
                break;
            case 4: // Twist
                geom_lattice->twisting(deformation_intensity * DEG2RAD, deformation_axis);
                geom_lattice->create();
                geom_mesh.create();
                update();
                emit meshChanged();
                break;
        }
    }
}

void MyGL::slot_Lattice_Bend() {
    Lattice_Mode = 0;
}

void MyGL::slot_Lattice_Squash() {
    Lattice_Mode = 1;
}

void MyGL::slot_Lattice_Stretch() {
    Lattice_Mode = 2;
}

void MyGL::slot_Lattice_Taper() {
    Lattice_Mode = 3;
}

void MyGL::slot_Lattice_Twist() {
    Lattice_Mode = 4;
}

void MyGL::slot_Lattice_X() {
    deformation_axis = 0;
}

void MyGL::slot_Lattice_Y() {
    deformation_axis = 1;
}

void MyGL::slot_Lattice_Z() {
    deformation_axis = 2;
}

void MyGL::slot_Onscreen_Controls(bool b) {
    onscreen_controls_active = b;
}

void MyGL::slot_Lattice_Show(bool b) {
    lattice_show = b;
}

void MyGL::slot_Deformation_Commit() {
    for(std::vector<Vertex*>::size_type v = 0; v < geom_mesh.vertices.size(); v++) {
        geom_mesh.vertices[v]->orig_pos = geom_mesh.vertices[v]->pos;
    }

    geom_lattice->recreateLattice();
    update();
}

void MyGL::slot_Onscreen_Translate() {
    Onscreen_Mode = 0;
}

void MyGL::slot_Onscreen_Rotate() {
    Onscreen_Mode = 1;
}

void MyGL::slot_Onscreen_Scale() {
    Onscreen_Mode = 2;
}
