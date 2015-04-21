#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QTextStream>
#include <QList>
#include <string>

// Color Values
vec4 BLACK = vec4(0, 0, 0, 1);
vec4 GREY = vec4(0.5, 0.5, 0.5, 1);
vec4 WHITE = vec4(1, 1, 1, 1);
vec4 RED = vec4(1, 0, 0, 1);
vec4 GREEN = vec4(0, 1, 0, 1);
vec4 BLUE = vec4(0, 0, 1, 1);

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
    geom_mesh.destroy();
    body->destroy();
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
    geom_mesh.faces = createCube();

    createJointGraph();
    geom_mesh.create();
    send_structures();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the wireframe shader
    prog_wire.create(":/glsl/wire.vert.glsl", ":/glsl/wire.frag.glsl");

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

//    send_structures();
//    geom_mesh.create();
//    createJoints(body);

    glm::mat4 model = glm::mat4();

    // Mesh
    prog_lambert.setModelMatrix(model);
    prog_lambert.draw(*this, geom_mesh);

    glDisable(GL_DEPTH_TEST);
    model = glm::mat4();
    if (f_selected != NULL) {
        f_selected->create(); // Probably a bad place to put this
        prog_wire.setModelMatrix(model);
        prog_wire.draw(*this, *f_selected);
    }
    if (h_selected != NULL) {
        h_selected->create(); // Probably a bad place to put this
        prog_wire.setModelMatrix(model);
        prog_wire.draw(*this, *h_selected);
    }
    if (v_selected != NULL) {
        v_selected->create(); // Probably a bad place to put this
        prog_wire.setModelMatrix(model);
        prog_wire.draw(*this, *v_selected);
    }
    traverseJoint(body);

    if (j_selected != NULL) {
        emit sig_sendJointTranslation(vec4toString(j_selected->getLocalTransformation() * vec4(0, 0, 0, 1)));
        emit sig_sendJointQuat(vec4toString(toMat4(j_selected->rot) * vec4(0, 0, 0, 1)));
    }

    glEnable(GL_DEPTH_TEST);
}

void MyGL::createJointGraph() {
    body = new Joint("root", NULL, vec4(0, 0, 0, 1), quat(0, 0, 0, 0));
    Joint* neck = new Joint("neck", body, vec4(0, -1, 0, 1), quat(0, 0, 0, 0));
    Joint* heck = new Joint("here", neck, vec4(-2, -1, 0, 1), quat(0, 0, 0, 0));

    createJoints(body);
    emit sig_sendItemTop(body);
}

void MyGL::createJoints(Joint* j) {
    j->create();

    for(std::vector<Joint*>::size_type i = 0; i < j->children.size(); i++) {
        createJoints(j->children[i]);
    }
}

void MyGL::traverseJoint(Joint* j) {
    for(std::vector<Joint*>::size_type i = 0; i < j->children.size(); i++) {
        traverseJoint(j->children[i]);
    }

    mat4 model = j->getOverallTransformation();
    prog_wire.setModelMatrix(model);
    prog_wire.draw(*this, *j);
}

void MyGL::send_structures() {
    // Clear the lists
    emit sig_ResetList();

    // Store all of the unique vertices
    QSet<Vertex*> verts = {};

    for(std::vector<Face*>::size_type i = 0; i < geom_mesh.faces.size(); i++) {
        // Send all of the faces
        emit sig_SendFace(geom_mesh.faces[i]);

        HalfEdge* curr = geom_mesh.faces[i]->start_edge;
        do {
            emit sig_SendHalfEdges(curr);
            verts.insert(curr->vert);
            curr = curr->next;
        } while (curr != geom_mesh.faces[i]->start_edge);
    }

    // Send all of the vertices
    QSet<Vertex*>::iterator i;
    for (i = verts.begin(); i != verts.end(); ++i) {
        emit sig_SendVertices(*i);
    }
}

///      4__________5  FACES         HALF EDGES
///     /|         /|  Front:  0  >> [0-4)
///    / |        / |  Top:    1  >> [4-8)
///   2__|_______3  |  Back:   2  >> [8-12)
///   |  |       |  |  Bottom: 3  >> [12-16)
///   |  6_______|__7  Right:  4  >> [16-20)
///   | /        | /   Left:   5  >> [20-24)
///   0__________1
///

vector<Face*> MyGL::createCube() {
    // Create vertices
    vector<Vertex*> verts = {};
    verts.push_back(new Vertex(num++, vec4(-0.5f, -0.5f,  0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4( 0.5f, -0.5f,  0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4(-0.5f,  0.5f,  0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4( 0.5f,  0.5f,  0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4(-0.5f,  0.5f, -0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4( 0.5f,  0.5f, -0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4(-0.5f, -0.5f, -0.5f, 1)));
    verts.push_back(new Vertex(num++, vec4( 0.5f, -0.5f, -0.5f, 1)));

    // Create empty faces
    vector<Face*> fs = {};
    for (int i = 0; i < 6; i++) {
        fs.push_back(new Face(num++));
        fs[i]->color = vec4(1, 1, 1, 1);
    }

    // Create empty half edges
    vector<HalfEdge*> halfedges = {};
    for (int i = 0; i < 48; i ++) {
        halfedges.push_back(new HalfEdge(num++));
    }

    // Add faces to half edge
    for (int i = 0; i < 6; i++) {
        // Every 4 half edges correspond to a face
        for (int j = i * 4; j < i * 4 + 4; j++) {
            halfedges[j]->face = fs[i];
        }

        // Set faces start edge
        fs[i]->start_edge = halfedges[i * 4];
    }

     // Set Vertices to Half Edges
     // Front
     halfedges[0]->vert = verts[1];
     halfedges[1]->vert = verts[3];
     halfedges[2]->vert = verts[2];
     halfedges[3]->vert = verts[0];

     // Top
     halfedges[4]->vert = verts[3];
     halfedges[5]->vert = verts[5];
     halfedges[6]->vert = verts[4];
     halfedges[7]->vert = verts[2];

     // Back
     halfedges[8]->vert = verts[4];
     halfedges[9]->vert = verts[5];
     halfedges[10]->vert = verts[7];
     halfedges[11]->vert = verts[6];

     // Bottom
     halfedges[12]->vert = verts[0];
     halfedges[13]->vert = verts[6];
     halfedges[14]->vert = verts[7];
     halfedges[15]->vert = verts[1];

     // Right
     halfedges[16]->vert = verts[1];
     halfedges[17]->vert = verts[7];
     halfedges[18]->vert = verts[5];
     halfedges[19]->vert = verts[3];

     // Left
     halfedges[20]->vert = verts[2];
     halfedges[21]->vert = verts[4];
     halfedges[22]->vert = verts[6];
     halfedges[23]->vert = verts[0];

     // Set HalfEdges to verts
     verts[0]->edge = halfedges[3];
     verts[1]->edge = halfedges[0];
     verts[2]->edge = halfedges[2];
     verts[3]->edge = halfedges[4];
     verts[4]->edge = halfedges[6];
     verts[5]->edge = halfedges[5];
     verts[6]->edge = halfedges[13];
     verts[7]->edge = halfedges[10];

     // Set Half Edge Nexts
     for (int i = 0; i < 24; i++) {
         if (i % 4 == 3) {
             halfedges[i]->next = halfedges[i - 3];
         } else {
             halfedges[i]->next = halfedges[i + 1];
         }
     }

     // Set Half Edge Sym
     // Front
    halfedges[0]->sym = halfedges[12];
    halfedges[1]->sym = halfedges[16];
    halfedges[2]->sym = halfedges[4];
    halfedges[3]->sym = halfedges[20];

    // Top
    halfedges[4]->sym = halfedges[2];
    halfedges[5]->sym = halfedges[19];
    halfedges[6]->sym = halfedges[9];
    halfedges[7]->sym = halfedges[21];

    // Back
    halfedges[8]->sym = halfedges[22];
    halfedges[9]->sym = halfedges[6];
    halfedges[10]->sym = halfedges[18];
    halfedges[11]->sym = halfedges[14];

    // Bottom
    halfedges[12]->sym = halfedges[0];
    halfedges[13]->sym = halfedges[23];
    halfedges[14]->sym = halfedges[11];
    halfedges[15]->sym = halfedges[17];

    // Right
    halfedges[16]->sym = halfedges[1];
    halfedges[17]->sym = halfedges[15];
    halfedges[18]->sym = halfedges[10];
    halfedges[19]->sym = halfedges[5];

    // Left
    halfedges[20]->sym = halfedges[3];
    halfedges[21]->sym = halfedges[7];
    halfedges[22]->sym = halfedges[8];
    halfedges[23]->sym = halfedges[13];

    // Set geom mesh faces
    return fs;
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
    } else if (e->key() == Qt::Key_H) {
        if (h_selected != NULL) {
            h_selected = h_selected->next;
            f_selected = h_selected->face;
        }
    } else if (e->key() == Qt::Key_J) {
        if (h_selected != NULL) {
            h_selected = h_selected->sym;
            f_selected = h_selected->face;
        }
    }
    camera.RecomputeEye();
    update();  // Calls paintGL, among other things
}

void MyGL::triangulate(Face* f) {
    if (f == NULL ||
            f->start_edge == f->start_edge->next->next->next) {
        cout << "Triangulating a triangle. I thought we were pass that.";
        return;
    }


    HalfEdge* HE_0 = f->start_edge;

    // Create HE_A, HE_B
    HalfEdge* HE_A = new HalfEdge(num++, HE_0->vert);
    HalfEdge* HE_B = new HalfEdge(num++, HE_0->next->next->vert);
    HE_A->sym = HE_B;
    HE_B->sym = HE_A;

    // Create FACE2
    Face* FACE1 = f;
    Face* FACE2 = new Face(num++, FACE1->color, HE_A);
    geom_mesh.faces.push_back(FACE2);

    // Update Face Pointers
    HE_A->face = FACE2;
    HE_0->next->face = FACE2;
    HE_0->next->next->face = FACE2;
    HE_B->face = FACE1;


    // Fix up pointers
    HE_A->next = HE_0->next;
    HE_B->next = HE_0->next->next->next;
    HE_0->next->next->next = HE_A;
    HE_0->next = HE_B;

    // Recursively triangulate until you get all triangles
    triangulate(f);
    send_structures();
    geom_mesh.create();
    update();
}

// Face, Centroid Half Edges
void MyGL::quadrangulate(Face* f, vector<HalfEdge*> hes) {
    for(std::vector<HalfEdge*>::size_type i = 0; i < hes.size(); i++) {
        HalfEdge* HE_0 = hes[i];
        // Create HE_A, HE_B (Midpoint to Centroid)
        HalfEdge* HE_A = new HalfEdge(num++,HE_0->vert); // Centroid to Mid
        HalfEdge* HE_B = new HalfEdge(num++, f->centroid); // Mid to Centriod
        HE_A->sym = HE_B;
        HE_B->sym = HE_A;

        // Update Half Edge pointers
        HE_A->next = HE_0->next;
        HE_0->next = HE_B;
    }

    // Getting the last edge
    hes[0]->next->next = hes[hes.size() - 1]->next->sym;
    hes[0]->face = f;
    hes[0]->next->face = f;
    hes[0]->next->next->face = f;
    hes[0]->next->next->next->face = f;

    f->centroid->edge = hes[0]->next;
    for(std::vector<HalfEdge*>::size_type i = 1; i < hes.size(); i++) {
        hes[i]->next->next = hes[(hes.size() - 1 + i) % hes.size()]->next->sym;

        // Creating new faces and pointers
        Face* FACE2 = new Face(num++, f->color, hes[i]);
        geom_mesh.faces.push_back(FACE2);

        hes[i]->face = FACE2;
        hes[i]->next->face = FACE2;
        hes[i]->next->next->face = FACE2;
        hes[i]->next->next->next->face = FACE2;
    }
}

// Slots
void MyGL::slot_Triangulate() {
    if (f_selected != NULL) {
        triangulate(f_selected);
    } else {
        cout << "Select a face to delete";
    }
}

void MyGL::slot_AddVertextoEdge() {
    if (h_selected == NULL) {
        return;
    }

    // a.vert = start vert, b.vert = end vert
    HalfEdge* h1 = h_selected->sym; // BA
    HalfEdge* h2 = h_selected; // AB

    Vertex* e = new Vertex(num++, mid(h_selected));

    // Create new Half Edges EA, EB
    // EA
    HalfEdge* h3 = new HalfEdge(num++, h1->vert, h1->face, h1->next, h2);
    h1->vert->edge = h3;
    // EB
    HalfEdge* h4 = new HalfEdge(num++, h2->vert, h2->face, h2->next, h1);
    h2->vert->edge = h4;

    // Update Pointers h1 AB to AE, h2 BA to BE
    h1->vert = e;
    h1->next = h3;
    h1->sym = h4;

    h2->vert = e;
    h2->next = h4;
    h2->sym = h3;

    // Assign halfedge to new vertex e
    e->edge = h1;

    send_structures();
    geom_mesh.create();
    update();
}

void MyGL::addMidpointtoEdge(HalfEdge* h) {
    // a.vert = start vert, b.vert = end vert
    HalfEdge* h1 = h->sym; // BA
    HalfEdge* h2 = h; // AB

    Vertex* e = new Vertex(num++, midpoint(h));

    // Create new Half Edges EA, EB
    // EA
    HalfEdge* h3 = new HalfEdge(num++, h1->vert, h1->face, h1->next, h2);
    h1->vert->edge = h3;
    // EB
    HalfEdge* h4 = new HalfEdge(num++, h2->vert, h2->face, h2->next, h1);
    h2->vert->edge = h4;

    // Update Pointers h1 AB to AE, h2 BA to BE
    h1->vert = e;
    h1->next = h3;
    h1->sym = h4;

    h2->vert = e;
    h2->next = h4;
    h2->sym = h3;

    // Assign halfedge to new vertex e
    e->edge = h1;
}

void MyGL::slot_DeleteVertex() {
    if (v_selected == NULL) {
        return;
    }

//    vector<HalfEdge*> hes = {};
//    QSet<Face*> f_adj = {};
//    bool toggle = true;

//    // Collect all edges insident to the vertex
//    HalfEdge* curr = v_selected->edge;
//    do {
//        f_adj.insert(curr->face);
//        hes.push_back(curr);
//        if (toggle) {
//            curr = curr->next;
//            toggle = !toggle;
//        } else {
//            curr = curr->sym;
//            toggle = !toggle;
//        }
//    } while (curr != v_selected->edge);

//    // Go through the faces
//    QSet<Face*>::iterator i;
//    for (i = f_adj.begin(); i != f_adj.end(); ++i) {
//    }

//    for(std::vector<HalfEdge*>::size_type i = 0; i < hes.size(); i++) {
//    }

//    update();
}

void MyGL::slot_ReceiveSelectedHalfEdge(QListWidgetItem* h) {
    h_selected = (HalfEdge*) h;
    f_selected = h_selected->face;
}

void MyGL::slot_ReceiveSelectedVertex(QListWidgetItem* v) {
    v_selected = (Vertex*) v;
    emit sig_Sendx(v_selected->pos[0]);
    emit sig_Sendy(v_selected->pos[1]);
    emit sig_Sendz(v_selected->pos[2]);
}

void MyGL::slot_ReceiveSelectedFace(QListWidgetItem* f) {
    f_selected = (Face*) f;
    h_selected = f_selected->start_edge;
    emit sig_SendColR(f_selected->color[0]);
    emit sig_SendColG(f_selected->color[1]);
    emit sig_SendColB(f_selected->color[2]);
}

void MyGL::slot_ReceiveSelectedJoint(QTreeWidgetItem* j, QTreeWidgetItem* k) {
    cout << "Clicked";
    j_selected = (Joint*) j;
    j_selected->selected = true;
    if (k != NULL) {
        ((Joint*) k)->selected = false;
    }
    createJoints(body);
    update();
}

void MyGL::slot_ReceiveR(QString r) {
    temp_col[0] = r;
}

void MyGL::slot_ReceiveG(QString r) {
    temp_col[1] = r;
}

void MyGL::slot_ReceiveB(QString r) {
    temp_col[2] = r;
}

void MyGL::slot_Receivez(QString r) {
    temp_vert[2] = r;
}

void MyGL::slot_Receivey(QString r) {
    temp_vert[1] = r;
}

void MyGL::slot_Receivex(QString r) {
    temp_vert[0] = r;
}

void MyGL::slot_RotateXJoint() {
    if (j_selected != NULL) {
        j_selected->rot = normalize(j_selected->rot * angleAxis(5.0f * DEG2RAD, vec3(1, 0, 0)));
    }
    createJoints(body);
    update();
}

void MyGL::slot_RotateYJoint() {
    if (j_selected != NULL) {
        j_selected->rot = normalize(j_selected->rot * angleAxis(5.0f * DEG2RAD, vec3(0, 1, 0)));
    }
    createJoints(body);
    update();
}

void MyGL::slot_RotateZJoint() {
    if (j_selected != NULL) {
        j_selected->rot = normalize(j_selected->rot * angleAxis(5.0f * DEG2RAD, vec3(0, 0, 1)));
    }
    createJoints(body);
    update();
}

void MyGL::slot_UpdateVertexPos() {
    if (v_selected == NULL) {
        return;
    }

    v_selected->pos = vec4(temp_vert[0].toDouble(),
            temp_vert[1].toDouble(),
            temp_vert[2].toDouble(),
            1);

    // Collect faces insident to vertex
    QSet<Face*> f_adj = {};
    bool toggle = true;
    HalfEdge* curr = v_selected->edge;

    do {
        f_adj.insert(curr->face);
        if (toggle) {
            curr = curr->next;
            toggle = !toggle;
        } else {
            curr = curr->sym;
            toggle = !toggle;
        }
    } while (curr != v_selected->edge);

    // Go through the faces
    QSet<Face*>::iterator i;
    for (i = f_adj.begin(); i != f_adj.end(); ++i) {
        if (!planarity(*i, 0.1)) {
            cout << "Face " << (*i)->id << "is NOT planar";
            triangulate(*i);
        }
    }
    send_structures();
    geom_mesh.create();
    update();
}

void MyGL::slot_UpdateFaceColor() {
    f_selected->color = vec4(temp_col[0].toDouble(),
            temp_col[1].toDouble(),
            temp_col[2].toDouble(),
            1);
    send_structures();
    geom_mesh.create();
    update();
}

void MyGL::slot_Subdivide() {
    vector<Vertex*> centroids = {};
    QSet<HalfEdge*> edges = {};
    QSet<Vertex*> orig_verts = {};
    vector<Face*> orig_faces = {};

    // Compute All Centroids and Edges
    for(std::vector<Face*>::size_type i = 0; i < geom_mesh.faces.size(); i++) {
        orig_faces.push_back(geom_mesh.faces[i]);

        HalfEdge* curr = geom_mesh.faces[i]->start_edge;
        double num_vert = 0.0;
        Vertex* temp_cent = new Vertex(num++, vec4(0.0, 0.0, 0.0, 1.0), NULL);

        // Iterate through all of the halfedges
        do {
            temp_cent->pos += curr->vert->pos;

            // While you're here create the Edge hashset here!
            if (!edges.contains(curr) && !edges.contains(curr->sym)) {
                edges.insert(curr);
            }

            // As well as verts
            orig_verts.insert(curr->vert);
//            cout << "^^^" << curr->vert->pos << "^^^";

            num_vert++;
            curr = curr->next;
        } while (curr != geom_mesh.faces[i]->start_edge);
        temp_cent->pos /= num_vert;
        temp_cent->pos[3] = 1;
        geom_mesh.faces[i]->centroid = temp_cent;
    }

    // Compute the midpoint of each edge in the mesh
    // Split Edges Here!
    QSet<HalfEdge*>::iterator hes;
    for (hes = edges.begin(); hes != edges.end(); ++hes) {
            addMidpointtoEdge(*hes); // Function in MyGL
    }

    // Smooth the original vertices
    QSet<Vertex*>::iterator vs;
    for (vs = orig_verts.begin(); vs != orig_verts.end(); ++vs) {
        // Count insident faces
        QSet<Face*> f_adj = {};
        QSet<Vertex*> v_midpoints = {};
        HalfEdge* curr = (*vs)->edge;

        float n = 0.0;
        vec4 sum_e = vec4(0, 0, 0, 0);
        vec4 sum_f = vec4(0, 0, 0, 0);

        // Travel through insident half edges
        //  Obtain list of midpoints and insident halfedges
        bool toggle = true;
        do {
            if (curr->face != NULL) {
                f_adj.insert(curr->face);
            }

            // cout << curr->vert->pos << "$$$";
            v_midpoints.insert(curr->vert);

            if (toggle) {
                curr = curr->next;
            } else {
                curr = curr->sym;
            }

            toggle = !toggle;
        } while (curr != (*vs)->edge);


        // Go through the faces
        QSet<Face*>::iterator iter_f;
        for (iter_f = f_adj.begin(); iter_f != f_adj.end(); ++iter_f) {
            sum_f += (*iter_f)->centroid->pos;
        }

        // Go through midpoints
        QSet<Vertex*>::iterator iter_v;
        for (iter_v = v_midpoints.begin(); iter_v != v_midpoints.end(); ++iter_v) {
            // cout << (*iter_v)->pos << "+";
            sum_e += (*iter_v)->pos;
            n++;
        }
        sum_e -= (*vs)->pos;
        n--;
        // Equation
        vec4 v_prime = ((n-2) * (*vs)->pos)/n + sum_e/(n*n) + sum_f/(n*n);
        v_prime[3] = 1.0;
        (*vs)->pos = v_prime;
    }

    // Quadrangulate
    for(std::vector<HalfEdge*>::size_type i = 0; i < orig_faces.size(); i++) {
        Face* FACE = orig_faces[i];
        vector<HalfEdge*> mids = {};

        HalfEdge* curr = FACE->start_edge;
        do {
            mids.push_back(curr);
            curr = curr->next->next;
        } while (curr != FACE->start_edge);

        quadrangulate(orig_faces[i], mids);
    }

    send_structures();
    geom_mesh.create();
    update();
}

void MyGL::slot_OpenFile() {
    // Don't need to be fancy
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    ".",
                                                    tr("Texts (*.obj)"));

    // All the stuff
    vector<Vertex*> obj_verts = {};
    vector<Face*> obj_faces = {};
    QMap<Vertex*, vector<HalfEdge*>> vert_to_he;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
       QTextStream in(&file);
       while (!in.atEnd())
       {
          QStringList line = in.readLine().split(" ");

          if (line.at(0) == "v") {
              vec4 pos = vec4(line.at(1).toDouble(),
                              line.at(2).toDouble(),
                              line.at(3).toDouble(),
                              1);
              Vertex* ver = new Vertex(num++, pos, NULL);
              obj_verts.push_back(ver);
          } else if (line.at(0) == "f") {

              Face* temp_face = new Face(num++, BLUE, NULL);
              vector<int> vert_idx = {};
              HalfEdge* curr;

              // Get all positions of the vertices for the face
              for (QStringList::iterator it = line.begin() + 1; it != line.end(); ++it) {
                  vert_idx.push_back((*it).split("/").at(0).toInt() - 1);
              }

              // Iterate through all of the indices of the verts of the face
              for(std::vector<int>::size_type i = 0; i < vert_idx.size() - 1; i++) {
                  Vertex* vert_A = obj_verts[vert_idx[i]];
                  Vertex* vert_B = obj_verts[vert_idx[i + 1]];

                  HalfEdge* HE_B = NULL;
                  HalfEdge* HE_A = NULL;

                  // Iterate through all HE to vert_A
                  for(std::vector<HalfEdge*>::size_type j = 0; j < vert_to_he.value(vert_B).size(); j++) {
                      HalfEdge* t = vert_to_he.value(vert_B)[j];
                      if (t->sym->vert == vert_A) {
                          HE_B = t;
                          HE_A = HE_B->sym;
                      }
                  }

                  // Else Create Own Half Edges
                  if (HE_B == NULL) {
                      HE_B = new HalfEdge(num++, vert_B);
                      HE_A = new HalfEdge(num++, vert_A);
                      HE_A->sym = HE_B;
                      HE_B->sym = HE_A;
                      if (!vert_to_he.contains(vert_B)) {
                          vert_to_he.insert(vert_B, {});
                      }
                      vert_to_he[vert_B].push_back(HE_B);
                      if (!vert_to_he.contains(vert_A)) {
                          vert_to_he.insert(vert_A, {});
                      }
                      vert_to_he[vert_A].push_back(HE_A);
                      vert_A->edge = HE_A;
                      vert_B->edge = HE_B;
                  }

                  HE_B->face = temp_face;

                  if (i == 0) {
                      curr = HE_B;
                      temp_face->start_edge = HE_B;
                  } else {
                      curr->next = HE_B;
                      curr = curr->next;
                  }
              }

              Vertex* vert_C = obj_verts[vert_idx[vert_idx.size() - 1]];
              Vertex* vert_D = obj_verts[vert_idx[0]];

              HalfEdge* HE_D = NULL;
              HalfEdge* HE_C = NULL;

              for(std::vector<HalfEdge*>::size_type j = 0; j < vert_to_he.value(vert_D).size(); j++) {
                  HalfEdge* t = vert_to_he.value(vert_D)[j];
                  if (t->sym->vert == vert_C) {
                      HE_D = t;
                      HE_C = HE_D->sym;
                  }
              }

              // Else Create Own Half Edges
              if (HE_D == NULL) {
                  HE_D = new HalfEdge(num++, vert_D);
                  HE_C = new HalfEdge(num++, vert_C);
                  HE_C->sym = HE_D;
                  HE_D->sym = HE_C;
                  if (!vert_to_he.contains(vert_D)) {
                      vert_to_he.insert(vert_D, {});
                  }
                  vert_to_he[vert_D].push_back(HE_D);
                  if (!vert_to_he.contains(vert_C)) {
                      vert_to_he.insert(vert_C, {});
                  }
                  vert_to_he[vert_C].push_back(HE_C);
                  vert_C->edge = HE_C;
                  vert_D->edge = HE_D;
              }

              HE_D->face = temp_face;

              curr->next = HE_D;
              HE_D->next = temp_face->start_edge;

              obj_faces.push_back(temp_face);
          }
       }

       emit sig_ClearList();
       geom_mesh.faces = obj_faces;
       geom_mesh.destroy();
       send_structures();
       geom_mesh.create();
       update();
    }
       file.close(); // Remember to close the file
}


void MyGL::slot_ExportFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to File"),
                                                    ".",
                                                    tr("Texts (*.obj)"));
    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream out(&file);

        /*
         * All of the things
         */
        QList<Vertex*> list_verts;
        vector<Face*> mesh_faces = geom_mesh.faces;

        // Store all of the unique vertices
        QSet<Vertex*> verts = {};
        for(std::vector<Face*>::size_type i = 0; i < mesh_faces.size(); i++) {
            HalfEdge* curr = mesh_faces[i]->start_edge;
            do {
                verts.insert(curr->vert);
                curr = curr->next;
            } while (curr != mesh_faces[i]->start_edge);
        }
        list_verts = QList<Vertex*>::fromSet(verts);

        // Store all of vertices
        QList<Vertex*>::iterator v;
        for (v = list_verts.begin(); v != list_verts.end(); ++v) {
            out << "v " << (*v)->pos[0] << " " <<
                           (*v)->pos[1] << " " <<
                           (*v)->pos[2] << "\n";
        }

        // Store all of the normals
        for(std::vector<Face*>::size_type i = 0; i < mesh_faces.size(); i++) {
            HalfEdge* curr = mesh_faces[i]->start_edge;

            vec4 nor = cross(curr->vert->pos - curr->sym->vert->pos,
                             curr->next->vert->pos - curr->vert->pos);
            out << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << "\n";
        }


        // Store all of the faces
        for(std::vector<Face*>::size_type i = 0; i < mesh_faces.size(); i++) {
            HalfEdge* curr = mesh_faces[i]->start_edge;
            std::string f_line = "f ";
            do {
                f_line.append(std::to_string(list_verts.indexOf(curr->vert) + 1));
                f_line.append("//");
                f_line.append(std::to_string(i));
                f_line.append(" ");
                curr = curr->next;
            } while (curr != mesh_faces[i]->start_edge);
            f_line = f_line.substr(0, f_line.length()-1);
            out << QString::fromStdString(f_line) << "\n";
        }
    }
    file.close();
}

void MyGL::slot_OpenJSON() {
    // Don't need to be fancy
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    ".",
                                                    tr("Texts (*.JSON)"));

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jdoc = QJsonDocument::fromJson(file.readAll());
        QJsonObject jobj = jdoc.object();
        Joint* root = JsonToJoint(jobj["root"].toObject(), NULL);
        body->destroy();
        body = root;
        createJoints(body);
        emit sig_sendItemTop(body);

        update();

    }
       file.close(); // Remember to close the file
}

Joint* MyGL::JsonToJoint(QJsonObject qj, Joint* j) {
    QString n = qj["name"].toString();
    QJsonArray qja = qj["pos"].toArray();
    vec4 position = vec4(qja[0].toDouble(), qja[1].toDouble(),
            qja[2].toDouble(), 1);
    QJsonArray qja_r = qj["rot"].toArray();
    quat rotation = angleAxis((float) qja_r[0].toDouble(),
            vec3(qja_r[1].toDouble(), qja_r[2].toDouble(), qja_r[3].toDouble()));
    Joint* curr = new Joint(n, j, position, rotation);

    // Recurse through children
    qja = qj["children"].toArray();
    foreach (const QJsonValue & value, qja)
    {
        JsonToJoint(value.toObject(), curr);
    }

    return curr;
}

// Add a button to your GUI that calls a mesh skinning function
// given a mesh and the root joint of a skeleton. We only require
// that the skinning function be a simple distance-based weight
// assignation, but you can make yours fancier if you so choose.
void MyGL::slot_MeshSkinningFunction() {
    cout << "IN MESH SKINNING FUNCTION\n Vertex: J1Name, J1Weight \n Vertex: J2Name, J2Weight\n";
    vector<Face*> mesh_faces = geom_mesh.faces;
    // Get every vertex of the geom_mesh
    QSet<Vertex*> verts = {};
    for(std::vector<Face*>::size_type i = 0; i < mesh_faces.size(); i++) {
        HalfEdge* curr = mesh_faces[i]->start_edge;
        do {
            verts.insert(curr->vert);
            curr = curr->next;
        } while (curr != mesh_faces[i]->start_edge);
    }

    // Iterate through all of the vertices and the joints
    // Rank the two best joints per vertex
    QSet<Vertex*>::iterator i;
    int count = 0;
    for (i = verts.begin(); i != verts.end(); ++i) {
        // Get shortest distanced joints and their distances from vertices
        (*i)->jointToweight = closestJoints(*i, body, QPair<QPair<Joint*, float>, QPair<Joint*, float>>(
                                                QPair<Joint*, float>(NULL, std::numeric_limits<float>::max()),
                                                QPair<Joint*, float>(NULL, std::numeric_limits<float>::max())));
        // Now normalize the distances for the weights
        float sum = (*i)->jointToweight.first.second + (*i)->jointToweight.second.second;
        (*i)->jointToweight.first.second  /= sum;
        (*i)->jointToweight.second.second /= sum;

        if (count < 10) {
            std::cout << (*i)->pos << ": " << (*i)->jointToweight.first.first->name.toStdString() << " " << (*i)->jointToweight.first.second << "\n";
            std::cout << (*i)->pos << ": " << (*i)->jointToweight.second.first->name.toStdString() << " " << (*i)->jointToweight.second.second << "\n\n";
            count++;
        }
    }

}

QPair<QPair<Joint*, float>, QPair<Joint*, float>> MyGL::closestJoints(Vertex* v, Joint* j, QPair<QPair<Joint*, float>, QPair<Joint*, float>> res) {
    // Go through all of the joint children (siemiese)
    for(std::vector<Joint*>::size_type i = 0; i < j->children.size(); i++) {
        QPair<QPair<Joint*, float>, QPair<Joint*, float>> temp_heap = closestJoints(v, j->children[i], res);
        if (temp_heap.first.second < res.second.second) {
            if (temp_heap.first.second < res.first.second) {
                res.second = res.first;
                res.first = temp_heap.first;

                if (temp_heap.second.second < res.second.second) {
                    res.second = temp_heap.second;
                }
            } else {
              res.second = temp_heap.first;
            }
        }
    }

    // Actually calculate
    float temp = distance(v->pos, j->pos);
    if (temp < res.second.second) {
        if (temp < res.first.second) {
            res.second = res.first;
            res.first = QPair<Joint*, float>(j, temp);
        } else {
            res.second = QPair<Joint*, float>(j, temp);
        }
    }
    return res;
}
