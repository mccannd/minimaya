#include "scene/mesh.h"

#include <iostream>

#include <la.h>

#include <assert.h>

#include <QHash>
#include <QSet>
#include <QFile>
#include <QPair>

Mesh::Mesh()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer),
      bufJID(QOpenGLBuffer::VertexBuffer),
      bufJWeight(QOpenGLBuffer::VertexBuffer)
{
}


glm::vec4 averagedNormal(Vertex* v)
{
    float numEdges = 0.0;
    glm::vec3 normalSum = glm::vec3(0, 0, 0);
    // get an edge that points to this vertex
    HalfEdge* incomingEdge = v->edge;
    HalfEdge* start = incomingEdge;

    do {
        // calculate normal between this edge and next edgeglm::vec4 a = edge->sym->vert->pos;
        glm::vec4 a = incomingEdge->sym->vert->pos;
        glm::vec4 b = incomingEdge->vert->pos;
        glm::vec4 c = incomingEdge->next->vert->pos;
        a = a - b;
        c = c - b;
        glm::vec3 a3 = glm::vec3(a[0], a[1], a[2]);
        glm::vec3 c3 = glm::vec3(c[0], c[1], c[2]);
        glm::vec3 cross3 = glm::cross(c3, a3);

        cross3 = glm::normalize(cross3);
        normalSum = normalSum + cross3;


        incomingEdge = incomingEdge->next->sym;
        numEdges++;
    } while (incomingEdge != start);

    // average the sum of normals
    normalSum = (1.0f / numEdges) * normalSum;
    // return this as a vec4
    return glm::vec4(normalSum[0], normalSum[1], normalSum[2], 0);

}

// refresh all information necessary for opengl to draw
void Mesh::updateBuffers()
{
    // clear the current buffers
    meshVertexPositions.clear();
    meshVertexNormals.clear();
    meshVertexColors.clear();
    meshIndices.clear();
    meshJointIDs.clear();
    meshJointWeights.clear();

    GLuint vID = 0; // ID for vertex indices buffer

    // repopulate buffers from faces
    for (unsigned int i = 0; i < faces.size(); i++) {
        Face* f = faces[i];

        //get color and start edge
        glm::vec4 color = f->color;
        HalfEdge* start = f->start_edge;

        // check for bad edge geometry
        assert(start != NULL);
        assert(start->next != NULL);
        assert(start->next->next != NULL);
        assert(start != start->next->next);
        assert(start != start->next);

        // save ID of first vertex -- this will create
        // a "fan" of triangles from this vertex
        GLuint fan = vID;
        HalfEdge* edge = start;


        // traverse all other vertices, each completing a triangle of IDs
        do {
            // calculate normal for this vertex
            glm::vec4 normal;

            if (smoothNormals) {
                normal = averagedNormal(edge->vert);
            } else {

            glm::vec4 a = edge->sym->vert->pos;
            glm::vec4 b = edge->vert->pos;
            glm::vec4 c = edge->next->vert->pos;
            a = a - b;
            c = c - b;
            glm::vec3 a3 = glm::vec3(a[0], a[1], a[2]);
            glm::vec3 c3 = glm::vec3(c[0], c[1], c[2]);
            glm::vec3 cross3 = glm::cross(a3, c3);
            cross3 = glm::normalize(cross3);

            // edge case: zero cross product
            if (cross3[0] == 0 && cross3[1] == 0 && cross3[2] == 0) {
                HalfEdge* nextEdge = edge->next;

                // continue until good cross is found
                while (cross3[0] == 0 && cross3[1] == 0 && cross3[2] == 0) {
                    a = nextEdge->sym->vert->pos;
                    b = nextEdge->vert->pos;
                    c = nextEdge->next->vert->pos;
                    a = a - b;
                    c = c - b;
                    a3 = glm::vec3(a[0], a[1], a[2]);
                    cross3 = glm::vec3(c[0], c[1], c[2]);
                    cross3 = glm::cross(a3, c3);
                    nextEdge = nextEdge->next;

                    if (nextEdge == edge) { break; }
                }
            }

            normal = glm::vec4(0.0 - cross3[0],
                    0.0 - cross3[1], 0.0 - cross3[2], 0);

            }


            meshVertexPositions.push_back(edge->vert->pos);
            meshVertexColors.push_back(color);
            meshVertexNormals.push_back(normal);
            meshJointIDs.push_back(edge->vert->jointIDs);
            meshJointWeights.push_back((edge->vert->weights));

            if (vID <= fan + 2) {
                // no completed triangle yet
                //     c
                //   / |
                // a - b
                meshIndices.push_back(vID);
            } else {
                // add a triangle of IDs
                // d - c
                // | / |  (add d: ID of a, c, d)
                // a - b
                meshIndices.push_back(fan);
                meshIndices.push_back(vID - 1);
                meshIndices.push_back(vID);
            }
            // push IDs of triangle formed by start, previous vert, current vert

            vID++; // increase ID

            // move on to next edge
            assert(edge->next!=NULL); // check for bad geometry
            edge = edge->next;
        } while(edge != start);
    }
}


void Mesh::create()
{

    updateBuffers();

    count = meshIndices.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(meshIndices.data(),
                    meshIndices.size() * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(meshVertexPositions.data(),
                    meshVertexPositions.size() * sizeof(glm::vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(meshVertexColors.data(),
                    meshVertexPositions.size() * sizeof(glm::vec4));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(meshVertexNormals.data(),
                    meshVertexPositions.size() * sizeof(glm::vec4));

    bufJID.create();
    bufJID.bind();
    bufJID.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufJID.allocate(meshJointIDs.data(),
                    meshJointIDs.size() * sizeof(glm::ivec2));

    bufJWeight.create();
    bufJWeight.bind();
    bufJWeight.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufJWeight.allocate(meshJointWeights.data(),
                        meshJointWeights.size() * sizeof(glm::vec2));
}

void Mesh::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
    bufJID.destroy();
    bufJWeight.destroy();
}

// destroy all mesh information
void Mesh::clearAll()
{
    for (unsigned int i = 0; i < faces.size(); i++) {
        if (faces[i] != NULL) {
            delete faces[i];
        }
    }

    for (unsigned int i = 0; i < vertices.size(); i++) {
        if (vertices[i] != NULL) {
            delete vertices[i];
        }
    }

    for (unsigned int i = 0; i < edges.size(); i++) {
        if (edges[i] != NULL) {
            delete edges[i];
        }
    }

    faces.clear();
    edges.clear();
    vertices.clear();
}

GLenum Mesh::drawMode()
{
    return GL_TRIANGLES;
}

int Mesh::elemCount()
{
    return count;
}

bool Mesh::bindIdx()
{
    return bufIdx.bind();
}

bool Mesh::bindPos()
{
    return bufPos.bind();
}

bool Mesh::bindNor()
{
    return bufNor.bind();
}

bool Mesh::bindCol()
{
    return bufCol.bind();
}

// skeleton bindings
bool Mesh::bindJID()
{
    return bufJID.bind();
}

bool Mesh::bindJWeight()
{
    return bufJWeight.bind();
}

/// --- Interface Interaction Functions ---

void Mesh::moveVertex(Vertex* v, float x, float y, float z)
{
    // check if the vertex is in this mesh
    if(std::find(vertices.begin(), vertices.end(), v)
            == vertices.end()) {
        return;
    }

    // reposition with new coordinates
    v->pos = glm::vec4(x, y, z, 1);
    v->orig_pos = v->pos;

    // update the buffers
    this->create();
}

void Mesh::recolorFace(Face *f, float r, float g, float b)
{
    f->color = glm::vec4(r, g, b, 0);
    this->create();
}

Vertex* Mesh::divideEdge(HalfEdge *e, bool updateBuffers)
{
    // check that the edge is in this mesh
    if(std::find(edges.begin(), edges.end(), e) == edges.end()) {
        return NULL;
    }

    // v1 ---e--> vn ------> v2
    // v1 <------ vn <e.sym- v2

    // create a new vertex on the center of this edge
    Vertex* v1 = e->sym->vert;
    Vertex* v2 = e->vert;

    glm::vec4 avg = glm::vec4(0.5f * (v1->pos[0] + v2->pos[0]),
            0.5f * (v1->pos[1] + v2->pos[1]),
            0.5f * (v1->pos[2] + v2->pos[2]), 1);
    Vertex* vn = new Vertex(avg[0], avg[1], avg[2],
            ++vertexID);
    vertices.push_back(vn);

    // create an edge from vn to v2
    HalfEdge* n2 = new HalfEdge(v2, e->face, ++edgeID);
    edges.push_back(n2);

    // create an edge from vn to v1
    HalfEdge* n1 = new HalfEdge(v1, e->sym->face, ++edgeID);
    edges.push_back(n1);

    // redirect edge flow
    e->vert = vn;
    e->sym->vert = vn;
    vn->edge = e;
    v1->edge = n1;
    v2->edge = n2;
    n2->next = e->next;
    n1->next = e->sym->next;
    e->next = n2;
    e->sym->next = n1;
    // sneakyness: triangulating this face will fan here
    if (e->face != NULL) {
        e->face->start_edge = e;
    }
    if (e->sym->face!= NULL) {
        e->sym->face->start_edge = e->sym;
    }

    n2->pair(e->sym);
    n1->pair(e);

    // update buffers
    if (updateBuffers) {
        this->create();
    }

    return vn;
}

void Mesh::triangulateFace(Face *f)
{
    // check that the face is contained within this mesh
    if(std::find(faces.begin(), faces.end(), f) == faces.end()) {
        return;
    }

    HalfEdge* e = f->start_edge;

    // check that this is not a tri
    if (e == e->next->next) {
        return;
    }

    Vertex* fan = e->vert;
    HalfEdge* start = e;

    do {
        // if this is the first or second vertex, continue
        while (e == start || e == start->next) {
            e = e->next;
        }

        // now this edge will point to a vertex v2
        // which makes a triangle with vertex f and v1
        // where v1 and v2 are the start and end points of e

        //  ...
        // f--v2
        // |\ e
        //...\v1

        // we assume that there is a valid tri or nothing
        // on the other side of edge (f to v1),
        // s.t. start->next->vert = v1

        Vertex* v2 = e->vert;

        // create an edge from fan to v2
        HalfEdge* f2 = new HalfEdge(v2, e->face, ++edgeID);
        edges.push_back(f2);

        // create an edge from v2 to fan
        HalfEdge* f2sym = new HalfEdge(fan, e->face, ++edgeID);
        edges.push_back(f2sym);

        // redirect edge flow
        f2->pair(f2sym);
        f2sym->next = start->next;
        f2->next = e->next;
        e->next = f2sym;
        start->next = f2;

        // create a new tri between f, v1, v2, leave the rest
        Face* tri = new Face(start->face->color, ++faceID);
        f2sym->face = tri;
        f2sym->next->face = tri;
        f2sym->next->next->face = tri;
        tri->start_edge = f2sym;
        faces.push_back(tri);

        // move to what was the next edge on the face
        e = f2->next;
    } while (e->next != start);

    this->create();
}

// helper: return the num of edges sharing this vertex
int incidentEdges(Vertex* v)
{
    int edges = 0;
    HalfEdge* s = v->edge;
    HalfEdge* e = s;
    do {
        // cycle through all edges pointing to v
        e = e->next->sym;
        edges++;
    } while (e != s);

    return edges;
}

void Mesh::deleteVertex(Vertex *v)
{
    // check that the face is contained within this mesh
    if(std::find(vertices.begin(), vertices.end(), v)
            == vertices.end()) {
        return;
    }

    // check if this is an isolated vertex
    if (v->edge == NULL) {
        // delete this and return
        for (unsigned int j = 0; j < vertices.size(); j++) {
            if (v == vertices[j]) {
                vertices.erase(vertices.begin() + j);
                break;
            }
        }

        return;
    }

    std::vector<HalfEdge*> de = {}; // edges to delete
    std::vector<Vertex*> dv = {}; // vertices to delete
    std::vector<Face*> df = {}; // faces to delete
    std::vector<Vertex*> tv = {}; // target vertices

    // add vertex to list for deletion
    dv.push_back(v);

    ///
    /// reach out from the deleted vertex and compile the lists
    ///

    HalfEdge* start = v->edge;
    HalfEdge* e1 = start;

    do {
        HalfEdge* e2 = e1->sym; // edge going out from v
        df.push_back(e2->face); // face incident to v
        while (incidentEdges(e2->vert) <= 2) {
            // this vertex is only on two edges
            // so the edge it's on and the vertex
            // need to be deleted along with the face
            de.push_back(e2);
            de.push_back(e2->sym);
            dv.push_back(e2->vert);
        }
        // now the edge should point to a vertex with >= 3 edges
        tv.push_back(e2->vert);
        // add the edge it's on to the list of deleted edges
        de.push_back(e2);
        de.push_back(e2->sym);

        // cycle origin to next incoming edge to v
        e1 = e1->next->sym;
    } while (e1 != start);

    ///
    /// link the edges that will remain when the faces are gone
    ///

    e1 = start->sym;
    // number of target vertices reached in traversal
    unsigned int foundVerts = 0;

    while (tv.size() > foundVerts) {
        if(std::find(tv.begin(), tv.end(), e1->vert) != tv.end()) {
            // if the very first outgoing edge reaches a target, do nothing
            // it will not be in the ring of edges so move on to the next one
            // e1.next will be the first edge on this ring
            if (e1 != start->sym) {
                // this edge points to a target vertex
                foundVerts++;
                // link this edge with the edge on the other side of target
                //       d  <-- deleted vertex
                //       |
                //  b -- t -- a <-- at.next to tb instead of td
                e1->next = e1->next->sym->next;

                // we must also ensure that the vertex doesn't
                // point to an edge that will be deleted
                e1->vert->edge = e1;
            }

        }

        e1->face = NULL; // the face this is on will be deleted
        e1 = e1->next; // continue traversing edges
    }

    // delete everything on the lists
    // by erasing them from the face, vert, edge vecs

    // faces
    for (unsigned int i = 0; i < df.size(); i++) {
        if (df[i] != NULL) {
            for (unsigned int j = 0; j < faces.size(); j++) {
                if (df[i] == faces[j]) {
                    faces.erase(faces.begin() + j);
                    break;
                }
            }
        }
    }
    // edges
    for (unsigned int i = 0; i < de.size(); i++) {
        if (de[i] != NULL) {
            for (unsigned int j = 0; j < edges.size(); j++) {
                if (de[i] == edges[j]) {
                    edges.erase(edges.begin() + j);
                    break;
                }
            }
        }
    }
    // vertices
    for (unsigned int i = 0; i < dv.size(); i++) {
        if (dv[i] != NULL) {
            for (unsigned int j = 0; j < vertices.size(); j++) {
                if (dv[i] == vertices[j]) {
                    vertices.erase(vertices.begin() + j);
                    break;
                }
            }
        }
    }

    tv.clear();
    dv.clear();
    de.clear();
    df.clear();

    // edge case: no faces left in mesh
    if (faces.size() == 0) {
        this->clearAll();
    }
    // recreate buffers
    this->create();

}

// helper: recursively produce a list of all joints
void jointList(std::vector<Joint*> &joints, Joint* j)
{
    joints.push_back(j);
    for (unsigned int i = 0; i < j->numChildren(); i++) {
        jointList(joints, j->getChild(i));
    }
}

// helper: find the euclidean distance between a joint and vertex
float jointToVertDistance(Joint* j, Vertex* v)
{
    // center of joint, in world space
    glm::vec4 jc = j->getOverallTransformation() * glm::vec4(0, 0, 0, 1);
    // position of vertex
    glm::vec4 vc = v->pos;

    float dist = (float) sqrt((jc[0] - vc[0]) * (jc[0] - vc[0]) +
                        (jc[1] - vc[1]) * (jc[1] - vc[1]) +
                        (jc[2] - vc[2]) * (jc[2] - vc[2]));

    return dist;
}

void Mesh::linearBinding(Joint *root)
{
    // compile a list of all joints to iterate
    std::vector<Joint*> allJoints;
    jointList(allJoints, root);

    // for every vertex
    for (unsigned int i = 0; i < vertices.size(); i++) {
        // search for two closest joints
        int firstID;
        int secondID;
        float firstDistance = 4096;
        float secondDistance = 4096;
        Vertex* currentVertex = vertices[i];

        // edge case: only one joint
        // map the joint with full influence
        if (allJoints.size() == 1) {
            currentVertex->jointIDs = glm::vec2(0, 0);
            currentVertex->weights = glm::vec2(0.5, 0.5);
            continue;
        }

        // linear search
        for (unsigned int j = 0; j < allJoints.size(); j++) {
            Joint* currentJoint = allJoints[j];
            float distance = jointToVertDistance(currentJoint,
                                                 currentVertex);

            if (distance < firstDistance) {
                secondID = firstID;
                firstID = j;
                secondDistance = firstDistance;
                firstDistance = distance;
            } else if (distance < secondDistance) {
                secondID = j;
                secondDistance = distance;
            }
        }

        // normalize the weights
        float sumDistance = firstDistance + secondDistance;
        firstDistance /= sumDistance; // make proportional weights
        secondDistance /= sumDistance;

        // map these joints to vertex weights
        currentVertex->jointIDs = glm::ivec2(firstID, secondID);
        currentVertex->weights = glm::vec2(secondDistance, firstDistance);
//        std::cout<<"Bound vertex " << currentVertex->getID() <<" to joints: <"
//                << firstID << ", " << secondID <<
//                   "> with weights: <" << secondDistance <<
//                   ", " << firstDistance << ">\n";

    }

    // create the bind matrix for all joints
    // bind matrix is the inverse of overall transformations at this time
    for (unsigned int i = 0; i < allJoints.size(); i++) {
        allJoints[i]->bind_matrix =
                la::inverse(allJoints[i]->getOverallTransformation());
    }

    this->create();
}


void Mesh::subdivide(QListWidget *edgeList, QListWidget *faceList,
                     QListWidget *vertList)
{

    // maps a centroid to each face
    QHash<Face*, Vertex*> centroids;

    // lists any points that are new
    QSet<Vertex*> midpoints;

    // lists any faces that are new
    QSet<Face*> newFaces;

    /// --- Create the centroids of all faces ---
    /// --- Step 1 of 4
    ///

    for (unsigned int i = 0; i < faces.size(); i++) {

        float x = 0;
        float y = 0;
        float z = 0;

        double vertexCount = 0.0;

        // average all points on this face to make centroid

        HalfEdge* start = faces[i]->start_edge;
        HalfEdge* e = start;
        do {
            Vertex* v = e->vert;
            x += v->pos[0];
            y += v->pos[1];
            z += v->pos[2];
            e = e->next;
            vertexCount++;
        } while (e != start);

        x /= vertexCount;
        y /= vertexCount;
        z /= vertexCount;

        Vertex* centroid = new Vertex(x, y, z, ++vertexID);

        centroids[faces[i]] = centroid;
        vertices.push_back(centroid);
    }

    /// --- Create midpoints on each edge ---
    /// --- Step 2 of 4
    ///

    for (unsigned int i = 0; i < faces.size(); i++) {

        HalfEdge* start = faces[i]->start_edge;
        HalfEdge* e = start;

        // each midpoint is an average of:
        // adjacent vertices and centroids
        e = start;
        do {

            // check if a midpoint is mapped already
            if (midpoints.contains(e->vert) ||
                    midpoints.contains(e->sym->vert)) {
                e = e->next;
                if (e == start) {
                    break;
                }
                continue;
            }

            float x = 0;
            float y = 0;
            float z = 0;

            double vertexCount = 2.0;
            // endpoint 2
            Vertex* v2 = e->vert;

            x += v2->pos[0];
            y += v2->pos[1];
            z += v2->pos[2];

            // endpoint 1
            Vertex* v1 = e->sym->vert;

            x += v1->pos[0];
            y += v1->pos[1];
            z += v1->pos[2];
            // if the edge is a boundary its face ptr is null
            if (e->face != NULL) {
                Vertex* c = centroids[e->face];
                x += c->pos[0];
                y += c->pos[1];
                z += c->pos[2];
                vertexCount++;
            }
            if (e->sym->face != NULL) {
                Vertex* c = centroids[e->sym->face];
                x += c->pos[0];
                y += c->pos[1];
                z += c->pos[2];
                vertexCount++;
            }

            x /= vertexCount;
            y /= vertexCount;
            z /= vertexCount;

            Vertex* mid = this->divideEdge(e, false);
            mid->pos = glm::vec4(x, y, z, 1);
            mid->orig_pos = mid->pos;

            // add the midpoint to the hashset
            midpoints.insert(mid);

            e = e->next;
        } while (e != start);
    }

    /// --- Adjust existing vertices to new positions
    /// --- Step 3 of 4
    ///

    for (unsigned int i = 0; i < vertices.size(); i++) {
        // get adjacent edges
        // from each edge get a midpoint
        // and get its faces centroid
        Vertex* v = vertices[i];


        // continue if this is actually a midpoint or centroid
        if (v->edge == NULL || midpoints.contains(v)) {
            continue;
        }

        //std::cout<<"\nworking on vertex "<< v->getID() << "\n";

        HalfEdge* start = v->edge->sym;
        HalfEdge* e = start;

        float midPointCount = 0.0;

        float x_centroid = 0.0;
        float y_centroid = 0.0;
        float z_centroid = 0.0;

        float x_midpoints = 0.0;
        float y_midpoints = 0.0;
        float z_midpoints = 0.0;

        do {
            Face* f = e->face;

            if (f != NULL) {
                Vertex* c = centroids[f];
                x_centroid += c->pos[0];
                y_centroid += c->pos[1];
                z_centroid += c->pos[2];
            }

            Vertex* m = e->vert;
            if (midpoints.contains(m)) {
                x_midpoints += m->pos[0];
                y_midpoints += m->pos[1];
                z_midpoints += m->pos[2];
                midPointCount++;
            } else {
                std::cout<<"worked on vertex connected to non-midpoint\n";
            }
            // go to next edge pointing from this vert
            e = e->sym->next;
        } while (e != start);

        // calculate the coordinates weighted by midpoints
        float x = (midPointCount - 2) * v->pos[0] / midPointCount;
        float y = (midPointCount - 2) * v->pos[1] / midPointCount;
        float z = (midPointCount - 2) * v->pos[2] / midPointCount;

        x += x_centroid / (midPointCount * midPointCount);
        y += y_centroid / (midPointCount * midPointCount);
        z += z_centroid / (midPointCount * midPointCount);

        x += x_midpoints / (midPointCount * midPointCount);
        y += y_midpoints / (midPointCount * midPointCount);
        z += z_midpoints / (midPointCount * midPointCount);

        // set the vert up!
        v->pos = glm::vec4(x, y, z, 1);
        v->orig_pos = v->pos;
    }

    /// --- Quadrangulate original faces ---
    /// --- Step 4 of 4
    ///

    for (unsigned int i = 0; i < faces.size(); i++) {
        Face* f = faces[i];

        if (newFaces.contains(f) || !centroids.contains(f)) {
            continue;
        }

        Vertex* centroid = centroids[faces[i]];
        HalfEdge* e = f->start_edge;

        while (!midpoints.contains(e->vert)) {
            e = e->next;
        }

        Vertex* start_vert = e->vert;

        do {
            // make a quad on from this edge
            // v --- 1 -e-
            // |     |
            // |     |
            // 2 --- c
            Vertex* m1 = e->vert;
            Vertex* m2 = e->next->next->vert;

            if (e->sym->vert == centroid) {
                // this is NOT the first face

                if(e->next->next->next->vert == centroid) {
                    // this is the last quad
                    // do not create any edges
                    // reroute edges
                    e->next->next->next->next = e;

                    // create a new face and reassign to edges
                    Face* newQuad = new Face(f->color, ++faceID);
                    newQuad->start_edge = e;
                    e->face = newQuad;
                    e->next->face = newQuad;
                    e->next->next->face = newQuad;
                    e->next->next->next->face = newQuad;

                    // put quad into lists
                    newFaces.insert(newQuad);
                    faces.push_back(newQuad);
                    //faceList->insertItem(faceList->size(), newQuad);

                    e = e->next->next->next->sym;
                } else {
                    // this is an intermediate quad
                    // create two edges between the 2nd midpoint and cent
                    HalfEdge* c_two = new HalfEdge(m2, ++edgeID);
                    HalfEdge* two_c = new HalfEdge(centroid, ++edgeID);
                    c_two->pair(two_c);
                    edges.push_back(c_two);
                    edges.push_back(two_c);

                    // redirect edges
                    c_two->next = e->next->next->next;
                    e->next->next->next = two_c;
                    two_c->next = e;

                    // create a new face and reassign to edges
                    Face* newQuad = new Face(f->color, ++faceID);
                    newQuad->start_edge = e;
                    e->face = newQuad;
                    e->next->face = newQuad;
                    e->next->next->face = newQuad;
                    e->next->next->next->face = newQuad;
                    //faceList->insertItem(faceList->size(), newQuad);

                    // put quad into lists
                    newFaces.insert(newQuad);
                    faces.push_back(newQuad);

                    // move on to next edge
                    e = c_two;
                }

            } else {
                // the is the FIRST face
                // create the edges between the midpoints and centroid
                HalfEdge* c_one = new HalfEdge(m1, ++edgeID);
                HalfEdge* one_c = new HalfEdge(centroid, ++edgeID);
                HalfEdge* c_two = new HalfEdge(m2, ++edgeID);
                HalfEdge* two_c = new HalfEdge(centroid, ++edgeID);
                c_one->pair(one_c);
                c_two->pair(two_c);
                edges.push_back(c_one);
                edges.push_back(one_c);
                edges.push_back(c_two);
                edges.push_back(two_c);

                // redirect edges
                c_one->next = e->next;
                two_c->next = c_one;
                c_two->next = e->next->next->next;
                e->next->next->next = two_c;
                e->next = one_c;

                // assign this face to appropriate edges
                f->start_edge = c_one;
                c_one->face = f;
                two_c->face = f;

                centroid->edge = two_c;

                // move to next edge
                e = c_two;
            }

        } while (e->vert != start_vert);
    }

    this->create();
}

/// --- Creation of arbitrary meshes ---

// god have mercy on your soul if you need this
void Mesh::arbitraryMesh(std::vector<Face*> f,
                         std::vector<Vertex*> v,
                         std::vector<HalfEdge*> e)
{
    this->destroy();
    this->clearAll();
    faces = f;
    edges = e;
    vertices = v;
    this->create();
}

void Mesh::parseObj(QString& fileName)
{


    // maps the id of a vertex to a vertex
    QHash<int, Vertex*> verts;
    // maps vertices (A, B) to the edge from A to B
    // the sym of that edge will have the key (B, A)
    QHash<QPair<Vertex*, Vertex*>, HalfEdge*>
            existingEdges;

    QFile mesh(fileName);

    if (mesh.open(QFile::ReadOnly)) {

        vertexID = 0;
        faceID = 0;
        edgeID = 0;
        clearAll();

        // open the stream and begin parsing
        QTextStream in(&mesh);
        while (!in.atEnd()) {
            QString line = in.readLine();

            if (line.isEmpty()) {
                continue;
            }

            QStringList dividedList = line.split(" ",
                                                QString::SkipEmptyParts);

            // check which type of line this is
            if (dividedList.at(0) == "v") {
                // found a vertex
                // v (posx) (posy) (posz)
                float x = dividedList.at(1).toFloat();
                float y = dividedList.at(2).toFloat();
                float z = dividedList.at(3).toFloat();
                Vertex* v = new Vertex(x, y, z, ++vertexID);
                vertices.push_back(v);
                verts[vertexID] = v;
            } else if (dividedList.at(0) == "f") {
                // found a face
                // f (v/vt/vn) (v/vt/vn) ...

                Face* f = new Face(++faceID);
                faces.push_back(f);

                HalfEdge* prev = NULL;
                Vertex* startPoint = NULL;
                Vertex* endPoint = NULL;

                for(int i = 1; i < dividedList.size(); i++) {
                    QStringList vert = dividedList.at(i).
                            split("/", QString::SkipEmptyParts);
                    int vID = vert.at(0).toInt();

                    // get a pair of start and end points
                    // continue the cycle
                    startPoint = endPoint;
                    endPoint = verts[vID];

                    if (i == 1) {
                        QStringList svert = dividedList.at(dividedList.size() - 1).
                                split("/", QString::SkipEmptyParts);
                        int svID = svert.at(0).toInt();
                        startPoint = verts[svID];
                    }

                    QPair<Vertex*,Vertex*> a_to_b(startPoint, endPoint);
                    QPair<Vertex*,Vertex*> b_to_a(endPoint, startPoint);
                    // see if an edge for this pair exists
                    if (existingEdges.contains(a_to_b)) {
                        // if it does, route the last edge to this one and continue
                        HalfEdge* e = existingEdges[a_to_b];
                        e->face = f;
                        if (prev != NULL) { prev->next = e; }
                        prev = e;
                    } else {
                        // if it does not, create new edges and map them
                        HalfEdge* e1 = new HalfEdge(endPoint, ++edgeID);
                        HalfEdge* e2 = new HalfEdge(startPoint, ++edgeID);

                        e1->face = f; // set up pointers
                        e1->pair(e2);
                        endPoint->edge = e1;
                        startPoint->edge = e2;

                        edges.push_back(e1); // add to the vectors
                        edges.push_back(e2);

                        if (prev != NULL) { prev->next = e1; }

                        existingEdges[a_to_b] = e1; // add to the map
                        existingEdges[b_to_a] = e2;

                        prev = e1;
                    }


                }

                // connect the last edge to the first
                QStringList svert = dividedList.at(dividedList.size() - 1).
                        split("/", QString::SkipEmptyParts);
                int svID = svert.at(0).toInt();
                startPoint = verts[svID];
                QStringList vert = dividedList.at(1).
                        split("/", QString::SkipEmptyParts);
                int vID = vert.at(0).toInt();
                endPoint = verts[vID];

                QPair<Vertex*,Vertex*> a_to_b(startPoint, endPoint);
                QPair<Vertex*,Vertex*> b_to_a(endPoint, startPoint);

                // see if an edge for this pair exists
                if (existingEdges.contains(a_to_b)) {
                    // if it does, route the last edge to this one and continue
                    HalfEdge* e = existingEdges[a_to_b];
                    e->face = f;
                    f->start_edge = e;
                    if (prev != NULL) { prev->next = e; }
                    prev = e;
                } else {
                    // if it does not, create new edges and map them
                    HalfEdge* e1 = new HalfEdge(endPoint, ++edgeID);
                    HalfEdge* e2 = new HalfEdge(startPoint, ++edgeID);

                    e1->face = f; // set up pointers
                    f->start_edge = e1;
                    e1->pair(e2);
                    endPoint->edge = e1;
                    startPoint->edge = e2;

                    edges.push_back(e1); // add to the vectors
                    edges.push_back(e2);

                    if (prev != NULL) { prev->next = e1; }

                    existingEdges[a_to_b] = e1; // add to the map
                    existingEdges[b_to_a] = e2;

                    prev = e1;
                }
            }
        }

        this->destroy();
        this->create();
    } else {
        // file failed to open
        std::cout<<"File failed to open\n";
        return;
    }

}

void Mesh::unitCube()
{
    vertexID = 0;
    faceID = 0;
    edgeID = 0;

    // positive z face
    // 1, 2, 3, 4
    Vertex* v1 = new Vertex(-0.5, -0.5, 0.5, ++vertexID);
    Vertex* v2 = new Vertex(0.5, -0.5, 0.5, ++vertexID);
    Vertex* v3 = new Vertex(0.5, 0.5, 0.5, ++vertexID);
    Vertex* v4 = new Vertex(-0.5, 0.5, 0.5, ++vertexID);

    Face* z1 = new Face(glm::vec4(1, 0, 0, 0), ++faceID);

    HalfEdge* e12 = new HalfEdge(v2, z1, ++edgeID);
    HalfEdge* e23 = new HalfEdge(v3, z1, ++edgeID);
    HalfEdge* e34 = new HalfEdge(v4, z1, ++edgeID);
    HalfEdge* e41 = new HalfEdge(v1, z1, ++edgeID);

    e12->next = e23;
    e23->next = e34;
    e34->next = e41;
    e41->next = e12;

    v1->edge = e41;
    v2->edge = e12;
    v3->edge = e23;
    v4->edge = e34;

    z1->start_edge = e12;

    // negative z face
    // 5, 8, 7, 6
    Vertex* v5 = new Vertex(-0.5, -0.5, -0.5, ++vertexID);
    Vertex* v6 = new Vertex(0.5, -0.5, -0.5, ++vertexID);
    Vertex* v7 = new Vertex(0.5, 0.5, -0.5, ++vertexID);
    Vertex* v8 = new Vertex(-0.5, 0.5, -0.5, ++vertexID);

    Face* z2 = new Face(glm::vec4(0, 1, 1, 0),++faceID);

    HalfEdge* e65 = new HalfEdge(v5, z2, ++edgeID);
    HalfEdge* e58 = new HalfEdge(v8, z2, ++edgeID);
    HalfEdge* e87 = new HalfEdge(v7, z2, ++edgeID);
    HalfEdge* e76 = new HalfEdge(v6, z2, ++edgeID);

    e65->next = e58;
    e58->next = e87;
    e87->next = e76;
    e76->next = e65;

    v5->edge = e65;
    v6->edge = e76;
    v7->edge = e87;
    v8->edge = e58;

    z2->start_edge = e65;

    // positive x face
    // 2, 6, 7, 3

    Face* x1 = new Face(glm::vec4(0, 1, 0, 0), ++faceID);
    HalfEdge* e26 = new HalfEdge(v6, x1, ++edgeID);
    HalfEdge* e67 = new HalfEdge(v7, x1, ++edgeID);
    HalfEdge* e73 = new HalfEdge(v3, x1, ++edgeID);
    HalfEdge* e32 = new HalfEdge(v2, x1, ++edgeID);

    e26->next = e67;
    e67->next = e73;
    e73->next = e32;
    e32->next = e26;

    x1->start_edge = e26;

    // positive y face
    // 3, 7, 8, 4

    Face* y1 = new Face(glm::vec4(0, 0, 1, 0), ++faceID);
    HalfEdge* e37 = new HalfEdge(v7, y1, ++edgeID);
    HalfEdge* e78 = new HalfEdge(v8, y1, ++edgeID);
    HalfEdge* e84 = new HalfEdge(v4, y1, ++edgeID);
    HalfEdge* e43 = new HalfEdge(v3, y1, ++edgeID);

    e37->next = e78;
    e78->next = e84;
    e84->next = e43;
    e43->next = e37;

    y1->start_edge = e37;

    // negative x face
    // 1, 4, 8, 5
    Face* x2 = new Face(glm::vec4(1, 0, 1, 0), ++faceID);
    HalfEdge* e14 = new HalfEdge(v4, x2, ++edgeID);
    HalfEdge* e48 = new HalfEdge(v8, x2, ++edgeID);
    HalfEdge* e85 = new HalfEdge(v5, x2, ++edgeID);
    HalfEdge* e51 = new HalfEdge(v1, x2, ++edgeID);

    e14->next = e48;
    e48->next = e85;
    e85->next = e51;
    e51->next = e14;

    x2->start_edge = e14;

    // negative y face
    // 1, 5, 6, 2

    Face* y2 = new Face(glm::vec4(1, 1, 0, 0), ++faceID);
    HalfEdge* e15 = new HalfEdge(v5, y2, ++edgeID);
    HalfEdge* e56 = new HalfEdge(v6, y2, ++edgeID);
    HalfEdge* e62 = new HalfEdge(v2, y2, ++edgeID);
    HalfEdge* e21 = new HalfEdge(v1, y2, ++edgeID);

    e15->next = e56;
    e56->next = e62;
    e62->next = e21;
    e21->next = e15;

    y2->start_edge = e15;

    // pair the edges
    e12->pair(e21);
    e14->pair(e41);
    e15->pair(e51);
    e23->pair(e32);
    e26->pair(e62);
    e34->pair(e43);
    e37->pair(e73);
    e48->pair(e84);
    e56->pair(e65);
    e58->pair(e85);
    e67->pair(e76);
    e78->pair(e87);

    // clear all of the data for this mesh
    // then add everything
    // EVERYTHING
    this->clearAll();

    faces.push_back(z1);
    faces.push_back(z2);
    faces.push_back(x1);
    faces.push_back(y1);
    faces.push_back(x2);
    faces.push_back(y2);

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);

    edges.push_back(e12);
    edges.push_back(e14);
    edges.push_back(e15);
    edges.push_back(e23);
    edges.push_back(e26);
    edges.push_back(e34);
    edges.push_back(e37);
    edges.push_back(e48);
    edges.push_back(e56);
    edges.push_back(e58);
    edges.push_back(e67);
    edges.push_back(e78);
    edges.push_back(e21);
    edges.push_back(e41);
    edges.push_back(e51);
    edges.push_back(e32);
    edges.push_back(e62);
    edges.push_back(e43);
    edges.push_back(e73);
    edges.push_back(e84);
    edges.push_back(e65);
    edges.push_back(e85);
    edges.push_back(e76);
    edges.push_back(e87);

    this->destroy();
    this->create();
}
