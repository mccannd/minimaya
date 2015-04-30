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

void Lattice::squashing(float q, int deformation_axis) {
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                int idx = i * (y + 1) * (z + 1) + j * (z + 1) + k;
                float X = ctrlpts[idx]->orig_pos[0];
                float Y = ctrlpts[idx]->orig_pos[1];
                float Z = ctrlpts[idx]->orig_pos[2];

                if (deformation_axis == 0) {
//                    float mid = (maxy - miny)/2;
//                    float curr = (Y - miny)/(maxy - miny) - mid;

//                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
//                            scale(mat4(), vec3((abs(Y) + q * q) * (abs(Y) + q * q),
//                                        1,
//                                        1));
                } else if (deformation_axis == 1) {
                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
                            mat4_cast(angleAxis(q * ((float) j)/y, vec3(0, 1, 0)));
                } else if (deformation_axis == 2) {
                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
                            mat4_cast(angleAxis(q * ((float) k)/z, vec3(0, 0, 1)));
                }
            }
        }
    }
    freeFormDeformation();
}

// q = angle in degrees
void Lattice::twisting(float q, int deformation_axis) {
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                int idx = i * (y + 1) * (z + 1) + j * (z + 1) + k;

                if (deformation_axis == 0) {
                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
                            mat4_cast(angleAxis(q * ((float) i)/x, vec3(1, 0, 0)));
                } else if (deformation_axis == 1) {
                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
                            mat4_cast(angleAxis(q * ((float) j)/y, vec3(0, 1, 0)));
                } else if (deformation_axis == 2) {
                    ctrlpts[idx]->pos = ctrlpts[idx]->orig_pos *
                            mat4_cast(angleAxis(q * ((float) k)/z, vec3(0, 0, 1)));
                }
            }
        }
    }
    freeFormDeformation();
}

// q is angle
void Lattice::bending(float q, int deformation_axis) {
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                int idx = i * (y + 1) * (z + 1) + j * (z + 1) + k;
                if (deformation_axis == 0) {
                    // Bend on X Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;

                    float theta = q * (X - Y0);

                    float y_prime;
                    float x_prime;

                    if (minx <= X && X <= maxx ) {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0;
                        y_prime = cos(theta) * (Y - 1/q) + (1/q);
                    } else if (X < minx) {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0 + cos(theta) * (X - Y0);
                        y_prime = cos(theta) * (Y - 1/q) + (1/q) + sin(theta) * (X - Y0);
                    } else {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0 + cos(theta) * (X - Y0);
                        y_prime = cos(theta) * (Y - 1/q) + (1/q) + sin(theta) * (X - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(x_prime, y_prime, Z, 1);
                } else if (deformation_axis == 1) {
                    // Bend on Y Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;
                    float theta = q * (Y - Y0);

                    float y_prime;
                    float z_prime;

                    if (miny <= Y && Y <= maxy ) {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0;
                        z_prime = cos(theta) * (Z - 1/q) + (1/q);
                    } else if (Y < miny) {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0 + cos(theta) * (Y - Y0);
                        z_prime = cos(theta) * (Z - 1/q) + (1/q) + sin(theta) * (Y - Y0);
                    } else {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0 + cos(theta) * (Y - Y0);
                        z_prime = cos(theta) * (Z - 1/q) + (1/q) + sin(theta) * (Y - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(X, y_prime, z_prime, 1);
                } else if (deformation_axis == 2) {
                    // Bend on Z Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;
                    float theta = q * (Z - Y0);

                    float x_prime;
                    float z_prime;

                    if (minz <= Z && Z <= maxz ) {
                        z_prime = -sin(theta) * (X - 1/q) + Y0;
                        x_prime = cos(theta) * (X - 1/q) + (1/q);
                    } else if (Z < minz) {
                        z_prime = -sin(theta) * (X - 1/q) + Y0 + cos(theta) * (Z - Y0);
                        x_prime = cos(theta) * (X - 1/q) + (1/q) + sin(theta) * (Z - Y0);
                    } else {
                        z_prime = -sin(theta) * (X - 1/q) + Y0 + cos(theta) * (Z - Y0);
                        x_prime = cos(theta) * (X - 1/q) + (1/q) + sin(theta) * (Z - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(x_prime, Y, z_prime, 1);
                }
            }
        }
    }
    freeFormDeformation();
}

// q is the amoutn to which we taper. q = [0, 1]
void Lattice::tapering(float q, int deformation_axis) {
    for (int i = 0; i <= x; i++) {
        for (int j = 0; j <= y; j++) {
            for (int k = 0; k <= z; k++) {
                int idx = i * (y + 1) * (z + 1) + j * (z + 1) + k;
                if (deformation_axis == 0) {
                    // Bend on X Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;

                    float theta = q * (X - Y0);

                    float y_prime;
                    float x_prime;

                    if (minx <= X && X <= maxx ) {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0;
                        y_prime = cos(theta) * (Y - 1/q) + (1/q);
                    } else if (X < minx) {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0 + cos(theta) * (X - Y0);
                        y_prime = cos(theta) * (Y - 1/q) + (1/q) + sin(theta) * (X - Y0);
                    } else {
                        x_prime = -sin(theta) * (Y - 1/q) + Y0 + cos(theta) * (X - Y0);
                        y_prime = cos(theta) * (Y - 1/q) + (1/q) + sin(theta) * (X - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(x_prime, Y, Z, 1);
                } else if (deformation_axis == 1) {
                    // Bend on Y Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;
                    float theta = q * (Y - Y0);

                    float y_prime;
                    float z_prime;

                    if (miny <= Y && Y <= maxy ) {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0;
                        z_prime = cos(theta) * (Z - 1/q) + (1/q);
                    } else if (Y < miny) {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0 + cos(theta) * (Y - Y0);
                        z_prime = cos(theta) * (Z - 1/q) + (1/q) + sin(theta) * (Y - Y0);
                    } else {
                        y_prime = -sin(theta) * (Z - 1/q) + Y0 + cos(theta) * (Y - Y0);
                        z_prime = cos(theta) * (Z - 1/q) + (1/q) + sin(theta) * (Y - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(X, y_prime, Z, 1);
                } else if (deformation_axis == 2) {
                    // Bend on Z Axis
                    float X = ctrlpts[idx]->orig_pos[0];
                    float Y = ctrlpts[idx]->orig_pos[1];
                    float Z = ctrlpts[idx]->orig_pos[2];

                    float Y0 = 0;
                    float theta = q * (Z - Y0);

                    float x_prime;
                    float z_prime;

                    if (minz <= Z && Z <= maxz ) {
                        z_prime = -sin(theta) * (X - 1/q) + Y0;
                        x_prime = cos(theta) * (X - 1/q) + (1/q);
                    } else if (Z < minz) {
                        z_prime = -sin(theta) * (X - 1/q) + Y0 + cos(theta) * (Z - Y0);
                        x_prime = cos(theta) * (X - 1/q) + (1/q) + sin(theta) * (Z - Y0);
                    } else {
                        z_prime = -sin(theta) * (X - 1/q) + Y0 + cos(theta) * (Z - Y0);
                        x_prime = cos(theta) * (X - 1/q) + (1/q) + sin(theta) * (Z - Y0);
                    }

                    ctrlpts[idx]->pos = vec4(X, Y, z_prime, 1);
                }
            }
        }
    }
    freeFormDeformation();


//    for (int i = 0; i <= x; i++) {
//        for (int j = 0; j <= y; j++) {
//            for (int k = 0; k <= z; k++) {
//                int idx = i * (y + 1) * (z + 1) + j * (z + 1) + k;

//                if (deformation_axis == 0) {
//                    float fx = (maxx - ctrlpts[idx]->orig_pos[0]) / (float) (maxx - minx);
//                    ctrlpts[idx]->pos = vec4(ctrlpts[idx]->orig_pos[0],
//                                             ctrlpts[idx]->orig_pos[1] * fx,
//                                             ctrlpts[idx]->orig_pos[2],
//                                             1);
//                } else if (deformation_axis == 1) {
//                    float fy = (maxy - ctrlpts[idx]->orig_pos[1]) / (float) (maxy - miny);
//                    ctrlpts[idx]->pos = vec4(ctrlpts[idx]->orig_pos[0],
//                                             ctrlpts[idx]->orig_pos[1],
//                                             ctrlpts[idx]->orig_pos[2] * fy,
//                                             1);
//                } else if (deformation_axis == 2) {
//                    float fz = q * (maxz - ctrlpts[idx]->orig_pos[0]) / (float) (maxz - minz);
//                    ctrlpts[idx]->pos = vec4(ctrlpts[idx]->orig_pos[0] * fz,
//                                             ctrlpts[idx]->orig_pos[1],
//                                             ctrlpts[idx]->orig_pos[2],
//                                             1);
//                }
//            }
//        }
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
