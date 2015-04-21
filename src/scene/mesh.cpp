#include "mesh.h"

mymath m = mymath();
// int num = 0;

Mesh::Mesh()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer) {
}

Mesh::Mesh(vector<Face*> f)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer) {
    faces = f;
}

void Mesh::create()
{
    vector<vec4> mesh_vert_pos = {};
    vector<vec4> mesh_vert_nor = {};
    vector<vec4> mesh_vert_col = {};
    vector<GLuint> mesh_idx = {};

    for(std::vector<Face*>::size_type i = 0; i < faces.size(); i++) {
        HalfEdge* curr = faces[i]->start_edge;

        vec4 nor = cross(curr->vert->pos - curr->sym->vert->pos,
                         curr->next->vert->pos - curr->vert->pos);

        int n = 0;
        int root = mesh_vert_pos.size();

        do {
            n++;
            mesh_vert_pos.push_back(curr->vert->pos);
            mesh_vert_nor.push_back(nor);
            mesh_vert_col.push_back(faces[i]->color);
            curr = curr->next;
        } while (curr != faces[i]->start_edge);

        for (int k = 1; k < n - 1; k++) {
            mesh_idx.push_back(root);
            mesh_idx.push_back(root + k);
            mesh_idx.push_back(root + k + 1);
        }
    }

    int MESH_IDX_COUNT = mesh_idx.size();
    int MESH_VERT_COUNT = mesh_vert_pos.size();

    // cout << MESH_IDX_COUNT << ":" << MESH_VERT_COUNT << "\n";

    count = MESH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(mesh_idx.data(), MESH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(mesh_vert_pos.data(), MESH_VERT_COUNT * sizeof(vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(mesh_vert_col.data(), MESH_VERT_COUNT * sizeof(vec4));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(mesh_vert_nor.data(), MESH_VERT_COUNT * sizeof(vec4));
}

void Mesh::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufNor.destroy();
    bufCol.destroy();
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

