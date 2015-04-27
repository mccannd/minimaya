#include "lattice.h"

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

    boundaries(mesh);
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
            }
        }
    }
}

// Calculates the boundaries of the lattice
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

    X0 = vec3(minx, miny, minz);
    S = vec3(maxx - minx, 0.f, 0.f);
    T = vec3(0.f, maxy - miny, 0.f);
    U = vec3(0.f, 0.f, maxz - minz);
}

// Recompute the lattice, creating a new lattice basically
void Lattice::recreateLattice() {
    // Recompute the boundaries if they've changed, not if more divisions
    // are made.
    if (!updating_divisions) {
        boundaries(mesh);
    }
    updating_divisions = false;

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
            }
        }
    }

    this->create();
}

// Called when changes to the lattice subdivisions are made
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
    recreateLattice();
}

void Lattice::twisting() {
//    for(std::vector<Vertex*>::size_type i = 0; i < ctrlpts.size(); i++) {

//        float extent = i %


//        // Twist around the Z-Axis
//        vec4 P = ctrlpts[i]->pos;
//        float x_prime = P[0] * cos(q) - P[1] * sin(q);
//        float y_prime = P[0] * sin(q) + P[1] * cos(q);

//        ctrlpts[i]->pos = vec4(x_prime, y_prime, P[2], 1);
//    }
//    freeFormDeformation();
}

// Free Form Deformation (FFD)
// Uses Bernstein Formula
void Lattice::freeFormDeformation() {
//    int n = ctrlpts.size() - 1;
    for(std::vector<Vertex*>::size_type v = 0; v < mesh->vertices.size(); v++) {
        vec3 X = vec3(mesh->vertices[v]->orig_pos);

        float s = dot(cross(T, U), (X - X0)) / (dot(cross(T, U), S));
        float t = dot(cross(S, U), (X - X0)) / (dot(cross(S, U), T));
        float u = dot(cross(S, T), (X - X0)) / (dot(cross(S, T), U));

        // New X
        vec3 x_prime = vec3(0.f, 0.f, 0.f);
        for (int i = 0; i <= x; i++) {
            vec3 sum_t = vec3(0.f, 0.f, 0.f);
            for (int j = 0; j <= y; j++) {
                vec3 sum_u = vec3(0.f, 0.f, 0.f);
                for (int k = 0; k <= z; k++) {
                    sum_u += binomialSpline(z, k, u) *
                             vec3(ctrlpts[i * (y + 1) * (z + 1) + j * (z + 1) + k]->pos);
                }
                sum_t += binomialSpline(y, j, t) * sum_u;
            }
            x_prime += binomialSpline(x, i, s) * sum_t;
        }

        mesh->vertices[v]->pos = vec4(x_prime, 1);
    }
}

// Binomial Spline
float Lattice::binomialSpline(int n, int i, float f) {
    return combination(n, i) * pow((1 - f), n - i) * pow(f, i);
}

// Creates the lattice
void Lattice::create() {
    vector<vec4> lattice_vert_pos = {};
    vector<vec4> lattice_vert_col = {};
    vector<GLuint> lattice_idx = {};

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

// skeleton bindings (that I don't use?)s
bool Lattice::bindJID()
{
    return bufJID.bind();
}

bool Lattice::bindJWeight()
{
    return bufJWeight.bind();
}
