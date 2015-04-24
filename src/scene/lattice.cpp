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

void Lattice::updateDivisions(int xdivs, int ydivs, int zdivs) {
    if (xdivs < 1) {
        x = 1;
    } else {
        x = xdivs;
    }
    if (ydivs < 1) {
        y = 1;
    } else {
        y = ydivs;
    }
    if (zdivs < 1) {
        z = 1;
    } else {
        z = zdivs;
    }

    updating_divisions = true;
    this->create();
}

void Lattice::create()
{
    if (!updating_divisions) {
        boundaries(mesh);
    }
    updating_divisions = false;
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

    int LATTICE_IDX_COUNT = lattice_idx.size();
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
