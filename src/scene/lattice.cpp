#include "lattice.h"

//Lattice::Lattice()
//    : bufIdx(QOpenGLBuffer::IndexBuffer),
//      bufPos(QOpenGLBuffer::VertexBuffer),
//      bufNor(QOpenGLBuffer::VertexBuffer),
//      bufCol(QOpenGLBuffer::VertexBuffer),
//      bufJID(QOpenGLBuffer::VertexBuffer),
//      bufJWeight(QOpenGLBuffer::VertexBuffer) {
//}

Lattice::Lattice(Mesh* m)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer),
      bufJID(QOpenGLBuffer::VertexBuffer),
      bufJWeight(QOpenGLBuffer::VertexBuffer) {
    mesh = m;
    x = 2;
    y = 2;
    z = 2;
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
    updating_divisions = false;
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

void Lattice::freeFormDeformation() {
    int n = mesh->vertices.size();
    for(std::vector<Vertex*>::size_type i = 0; i < n; i++) {
        vec4 x = mesh->vertices[i]->pos;

        vec4 sum_s = vec4(0, 0, 0, 0);
        vec4 sum_t = vec4(0, 0, 0, 0);
        vec4 sum_u = vec4(0, 0, 0, 0);

        for (int i = 0; i < n; i++) {
            sum_s += binomialSpline(n, i, (x[0])) * ctrlpts[i]->pos; // s
            sum_t += binomialSpline(n, i, (x[1])) * ctrlpts[i]->pos; // t
            sum_u += binomialSpline(n, i, (x[2])) * ctrlpts[i]->pos; // u
        }

        mesh->vertices[i]->pos = sum_s * sum_t * sum_u;
    }


}

float Lattice::binomialSpline(int n, int i, float f) {
    return combination(n, i) * pow(f, n) * pow((1 - f), n - i);
}

void Lattice::create()
{
    if (!updating_divisions) {
        boundaries(mesh);
    }

    vector<vec4> lattice_vert_pos = {};
    vector<vec4> lattice_vert_col = {};
    vector<GLuint> lattice_idx = {};

    ctrlpts.erase(ctrlpts.begin(), ctrlpts.end());
    int num = 0;

    // All vertex positions
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                vec4 temp = vec4((maxx - minx)/x * i + minx,
                                 (maxy - miny)/y * j + miny,
                                 (maxz - minz)/z * k + minz,
                                 1);

                ctrlpts.push_back(new Vertex(temp, num++));
                // lattice_vert_pos.push_back(temp);
            }
        }
    }

    for(std::vector<Vertex*>::size_type i = 0; i < ctrlpts.size(); i++) {
        lattice_vert_pos.push_back(ctrlpts[i]->pos);
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
