#include "halfedge.h"

HalfEdge::HalfEdge(int i) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer)
{
    id = i;
    QListWidgetItem::setText(QString::number(id));
}

HalfEdge::HalfEdge(int i, Vertex *v, Face *f, HalfEdge *n, HalfEdge *s)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer){
    id = i;
    vert = v;
    face = f;
    next = n;
    sym = s;
    QListWidgetItem::setText(QString::number(id));
}

HalfEdge::HalfEdge(int i, Vertex *v, Face *f)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer){
    id = i;
    vert = v;
    face = f;
    next = NULL;
    sym = NULL;
    QListWidgetItem::setText(QString::number(id));
}

HalfEdge::HalfEdge(int i, Vertex *v)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer){
    id = i;
    vert = v;
    face = NULL;
    next = NULL;
    sym = NULL;
    QListWidgetItem::setText(QString::number(id));
}

void HalfEdge::create()
{

    int HALFEDGE_IDX_COUNT = 2;
    int HALFEDGE_VERT_COUNT = 2;

    vector<vec4> halfedge_vert_pos = {};
    vector<vec4> halfedge_vert_col = {};
    vector<GLuint> halfedge_idx = {};

    halfedge_idx.push_back(0);
    halfedge_idx.push_back(1);
    halfedge_vert_pos.push_back(sym->vert->pos);
    halfedge_vert_pos.push_back(vert->pos);
    halfedge_vert_col.push_back(vec4(0, 0, 0, 1));
    halfedge_vert_col.push_back(vec4(1, 1, 1, 1));

    count = HALFEDGE_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(halfedge_idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(halfedge_vert_pos.data(), HALFEDGE_VERT_COUNT * sizeof(vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(halfedge_vert_col.data(), HALFEDGE_VERT_COUNT * sizeof(vec4));
}

void HalfEdge::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufCol.destroy();
}

GLenum HalfEdge::drawMode()
{
    return GL_LINES;
}

int HalfEdge::elemCount()
{
    return count;
}

bool HalfEdge::bindIdx()
{
    return bufIdx.bind();
}

bool HalfEdge::bindPos()
{
    return bufPos.bind();
}

bool HalfEdge::bindNor()
{
    return bufNor.bind();
}

bool HalfEdge::bindCol()
{
    return bufCol.bind();
}
