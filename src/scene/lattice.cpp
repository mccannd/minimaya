#include "lattice.h"

//Lattice::Lattice()
//    : bufIdx(QOpenGLBuffer::IndexBuffer),
//      bufPos(QOpenGLBuffer::VertexBuffer),
//      bufNor(QOpenGLBuffer::VertexBuffer),
//      bufCol(QOpenGLBuffer::VertexBuffer),
//      bufJID(QOpenGLBuffer::VertexBuffer),
//      bufJWeight(QOpenGLBuffer::VertexBuffer) {
//}

Lattice::Lattice(Mesh* m, int a, int b, int c)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer),
      bufJID(QOpenGLBuffer::VertexBuffer),
      bufJWeight(QOpenGLBuffer::VertexBuffer) {
    mesh = m;
    x = a;
    y = b;
    z = c;

    boundaries(m);
}

void Lattice::boundaries(Mesh* m) {
    for(std::vector<Vertex*>::size_type i = 0; i < m->vertices.size(); i++) {
        vec4 pos = m->vertices[i]->pos;

        if (pos[0] > maxx) {
            maxx = pos[0];
        }
        if (pos[0] < minx) {
            minx = pos[0];
        }

        if (pos[1] > maxy) {
            maxy = pos[1];
        }
        if (pos[1] < miny) {
            miny = pos[1];
        }

        if (pos[2] > maxz) {
            maxz = pos[2];
        }
        if (pos[2] < minz) {
            minz = pos[2];
        }
    }
}

void Lattice::create()
{
    vector<vec4> lattice_vert_pos = {};
//    vector<vec4> lattice_vert_nor = {};
    vector<vec4> lattice_vert_col = {};
    vector<GLuint> lattice_idx = {};

    // All vertex positions
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                vec4 temp = vec4((maxx - minx)/x * i + minx,
                                 (maxy - miny)/y * j + miny,
                                 (maxz - minz)/z * k + minz,
                                 1);
                lattice_vert_pos.push_back(temp);

                cout << temp;
            }
        }
    }

    int LATTICE_VERT_COUNT = lattice_vert_pos.size();

    // Color
    for(int i = 0; i < LATTICE_VERT_COUNT; i++) {
        lattice_vert_col.push_back(vec4(0, 1, 0, 1));
    }

    // Indices for lines
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                // Creates all lines across X
                if (i != x) {
                    lattice_idx.push_back(i * (y + 1) * (z + 1) + j * (z + 1) + k);
                    lattice_idx.push_back((i + 1) * (y + 1) * (z + 1) + j * (z + 1) + k);
                }

                // Creates all lines across Y
                if (j != y) {
                    lattice_idx.push_back(i * (y + 1) * (z + 1) + j * (z + 1) + k);
                    lattice_idx.push_back(i * (y + 1) * (z + 1) + (j + 1) * (z + 1) + k);
                }

                // Creates all lines across Z
                if (k != z) {
                    lattice_idx.push_back(i * (y + 1) * (z + 1) + j * (z + 1) + k);
                    lattice_idx.push_back(i * (y + 1) * (z + 1) + j * (z + 1) + k + 1);
                }
            }
        }
    }
//    for (int i = 0; i < LATTICE_VERT_COUNT - 1; i++) {
//        lattice_idx.push_back(i);
//        lattice_idx.push_back(i + 1);
//    }

//    lattice_idx[(LATTICE_VERT_COUNT - 1) * 2] = LATTICE_VERT_COUNT - 1;
//    lattice_idx[(LATTICE_VERT_COUNT - 1) * 2 + 1] = 0;


//    for(std::vector<Face*>::size_type i = 0; i < faces.size(); i++) {
//        HalfEdge* curr = faces[i]->start_edge;

//        vec4 nor = cross(curr->vert->pos - curr->sym->vert->pos,
//                         curr->next->vert->pos - curr->vert->pos);

//        int n = 0;
//        int root = lattice_vert_pos.size();

//        do {
//            n++;
//            lattice_vert_pos.push_back(curr->vert->pos);
//            lattice_vert_nor.push_back(nor);
//            lattice_vert_col.push_back(faces[i]->color);
//            curr = curr->next;
//        } while (curr != faces[i]->start_edge);

//        for (int k = 1; k < n - 1; k++) {
//            lattice_idx.push_back(root);
//            lattice_idx.push_back(root + k);
//            lattice_idx.push_back(root + k + 1);
//        }
//    }

    int LATTICE_IDX_COUNT = lattice_idx.size();

    // cout << LATTICE_IDX_COUNT << ":" << LATTICE_VERT_COUNT << "\n";

    count = LATTICE_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(lattice_idx.data(), LATTICE_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(lattice_vert_pos.data(), LATTICE_VERT_COUNT * sizeof(vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(lattice_vert_col.data(), LATTICE_VERT_COUNT * sizeof(vec4));

//    bufNor.create();
//    bufNor.bind();
//    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    bufNor.allocate(lattice_vert_nor.data(), LATTICE_VERT_COUNT * sizeof(vec4));
}

void Lattice::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
}

GLenum Lattice::drawMode()
{
    return GL_LINES;
}

int Lattice::elemCount()
{
    return count;
}

bool Lattice::bindIdx()
{
    return bufIdx.bind();
}

bool Lattice::bindPos()
{
    return bufPos.bind();
}

bool Lattice::bindNor()
{
    return bufNor.bind();
}

bool Lattice::bindCol()
{
    return bufCol.bind();
}

// skeleton bindings
bool Lattice::bindJID()
{
    return bufJID.bind();
}

bool Lattice::bindJWeight()
{
    return bufJWeight.bind();
}
