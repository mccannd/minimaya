#include "Mesh.h"

#include <iostream>

#include <la.h>

#include <assert.h>


static const int CYL_IDX_COUNT = 240;
static const int CYL_VERT_COUNT = 80;

Mesh::Mesh()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
}


// refresh all information necessary for opengl to draw
void Mesh::updateBuffers()
{
    // clear the current buffers
    meshVertexPositions.clear();
    meshVertexNormals.clear();
    meshVertexColors.clear();
    meshIndices.clear();

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

            glm::vec4 normal = glm::vec4(cross3[0], cross3[1], cross3[2], 0);


            meshVertexPositions.push_back(edge->vert->pos);
            meshVertexColors.push_back(color);
            meshVertexNormals.push_back(normal);

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
}

void Mesh::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
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

    // update the buffers
    this->create();
}

void Mesh::recolorFace(Face *f, float r, float g, float b)
{
    f->color = glm::vec4(r, g, b, 0);
    this->create();
}

void Mesh::divideEdge(HalfEdge *e)
{
    // check that the edge is in this mesh
    if(std::find(edges.begin(), edges.end(), e) == edges.end()) {
        return;
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
    HalfEdge* n2 = new HalfEdge(v2, e->face, ++vertexID);
    edges.push_back(n2);

    // create an edge from vn to v1
    HalfEdge* n1 = new HalfEdge(v1, e->sym->face, ++vertexID);
    edges.push_back(n1);

    // redirect edge flow
    e->vert = vn;
    e->sym->vert = vn;
    vn->edge = e;
    n2->next = e->next;
    n1->next = e->sym->next;
    e->next = n2;
    e->sym->next = n1;
    // sneakyness: triangulating this face will fan here
    e->face->start_edge = e;
    e->sym->face->start_edge = e->sym;
    n2->pair(e->sym);
    n1->pair(e);

    // update buffers
    this->create();
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

void Mesh::unitCube()
{
    vertexID = 0;
    faceID = 0;
    edgeID = 0;

    // positive z face
    // 1, 4, 3, 2
    Vertex* v1 = new Vertex(-0.5, -0.5, 0.5, ++vertexID);
    Vertex* v2 = new Vertex(0.5, -0.5, 0.5, ++vertexID);
    Vertex* v3 = new Vertex(0.5, 0.5, 0.5, ++vertexID);
    Vertex* v4 = new Vertex(-0.5, 0.5, 0.5, ++vertexID);

    Face* z1 = new Face(glm::vec4(1, 0, 0, 0), ++faceID);

    HalfEdge* e21 = new HalfEdge(v1, z1, ++edgeID);
    HalfEdge* e32 = new HalfEdge(v2, z1, ++edgeID);
    HalfEdge* e43 = new HalfEdge(v3, z1, ++edgeID);
    HalfEdge* e14 = new HalfEdge(v4, z1, ++edgeID);
    e21->next = e14;
    e32->next = e21;
    e43->next = e32;
    e14->next = e43;

    v1->edge = e21;
    v2->edge = e32;
    v3->edge = e43;
    v4->edge = e14;

    z1->start_edge = e21;

    // negative z face
    // 5, 6, 7, 8
    Vertex* v5 = new Vertex(-0.5, -0.5, -0.5, ++vertexID);
    Vertex* v6 = new Vertex(0.5, -0.5, -0.5, ++vertexID);
    Vertex* v7 = new Vertex(0.5, 0.5, -0.5, ++vertexID);
    Vertex* v8 = new Vertex(-0.5, 0.5, -0.5, ++vertexID);

    Face* z2 = new Face(glm::vec4(1, 1, 0, 0),++faceID);

    HalfEdge* e56 = new HalfEdge(v6, z2, ++edgeID);
    HalfEdge* e67 = new HalfEdge(v7, z2, ++edgeID);
    HalfEdge* e78 = new HalfEdge(v8, z2, ++edgeID);
    HalfEdge* e85 = new HalfEdge(v5, z2, ++edgeID);
    e56->next = e67;
    e67->next = e78;
    e78->next = e85;
    e85->next = e56;

    v5->edge = e85;
    v6->edge = e56;
    v7->edge = e67;
    v8->edge = e78;

    z2->start_edge = e56;

    // positive x face
    // 2, 3, 7, 6

    Face* x1 = new Face(glm::vec4(0, 1, 0, 0), ++faceID);
    HalfEdge* e23 = new HalfEdge(v3, x1, ++edgeID);
    HalfEdge* e37 = new HalfEdge(v7, x1, ++edgeID);
    HalfEdge* e76 = new HalfEdge(v6, x1, ++edgeID);
    HalfEdge* e62 = new HalfEdge(v2, x1, ++edgeID);
    e23->next = e37;
    e37->next = e76;
    e76->next = e62;
    e62->next = e23;

    x1->start_edge = e23;

    // positive y face
    // 3, 4, 8, 7

    Face* y1 = new Face(glm::vec4(0, 0, 1, 0), ++faceID);
    HalfEdge* e34 = new HalfEdge(v4, y1, ++edgeID);
    HalfEdge* e48 = new HalfEdge(v8, y1, ++edgeID);
    HalfEdge* e87 = new HalfEdge(v7, y1, ++edgeID);
    HalfEdge* e73 = new HalfEdge(v3, y1, ++edgeID);
    e34->next = e48;
    e48->next = e87;
    e87->next = e73;
    e73->next = e34;

    y1->start_edge = e34;

    // negative x face
    // 1, 5, 8, 4
    Face* x2 = new Face(glm::vec4(0, 1, 1, 0), ++faceID);
    HalfEdge* e15 = new HalfEdge(v5, x2, ++edgeID);
    HalfEdge* e58 = new HalfEdge(v8, x2, ++edgeID);
    HalfEdge* e84 = new HalfEdge(v4, x2, ++edgeID);
    HalfEdge* e41 = new HalfEdge(v1, x2, ++edgeID);
    e15->next = e58;
    e58->next = e84;
    e84->next = e41;
    e41->next = e15;

    x2->start_edge = e15;

    // negative y face
    // 1, 2, 6, 5

    Face* y2 = new Face(glm::vec4(1, 0, 1, 0), ++faceID);
    HalfEdge* e12 = new HalfEdge(v2, y2, ++edgeID);
    HalfEdge* e26 = new HalfEdge(v6, y2, ++edgeID);
    HalfEdge* e65 = new HalfEdge(v5, y2, ++edgeID);
    HalfEdge* e51 = new HalfEdge(v1, y2, ++edgeID);
    e12->next = e26;
    e26->next = e65;
    e65->next = e51;
    e51->next = e12;

    y2->start_edge = e12;

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
