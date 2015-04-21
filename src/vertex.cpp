#include "vertex.h"


Vertex::Vertex(int i) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer)
{
    id = i;
    QListWidgetItem::setText(QString::number(id));
}

Vertex::Vertex(int i, vec4 p) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer)
{
    pos = p;
    id = i;
    QListWidgetItem::setText(QString::number(id));
}

Vertex::Vertex(int i, vec4 p, HalfEdge* e) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer)
{
    pos = p;
    edge = e;
    id = i;
    QListWidgetItem::setText(QString::number(id));
}


void Vertex::create()
{
//    vector<vec4> vert_vert_pos = {};
//    vert_vert_pos.push_back(pos);
//    vector<vec4> vert_vert_col = {};
//    vert_vert_col.push_back(vec4(1, 1, 0, 1));
//    vector<GLuint> vert_idx = {};
//    vert_idx.push_back(0);

//    bufIdx.create();
//    bufIdx.bind();
//    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    bufIdx.allocate(vert_idx.data(), sizeof(GLuint));

//    bufPos.create();
//    bufPos.bind();
//    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    bufPos.allocate(vert_vert_pos.data(), sizeof(vec4));

//    bufCol.create();
//    bufCol.bind();
//    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    bufCol.allocate(vert_vert_col.data(), sizeof(vec4));

    GLuint ray_idx[1];
    vec4 ray_vert_pos[1];
    vec4 ray_vert_col[1];

    ray_idx[0] = 0;
    ray_vert_pos[0] = pos;
    ray_vert_col[0] = vec4(1, 1, 0, 1);

    count = 1;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(ray_idx, sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(ray_vert_pos, sizeof(vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(ray_vert_col, sizeof(vec4));
}

void Vertex::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufCol.destroy();
}

GLenum Vertex::drawMode()
{
    return GL_POINTS;
}

int Vertex::elemCount()
{
    return count;
}

bool Vertex::bindIdx()
{
    return bufIdx.bind();
}

bool Vertex::bindPos()
{
    return bufPos.bind();
}

bool Vertex::bindNor()
{
    return bufNor.bind();
}

bool Vertex::bindCol()
{
    return bufCol.bind();
}
