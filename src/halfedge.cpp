#include "halfedge.h"


HalfEdge::HalfEdge(int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    this->id = id;
    QString s = QString::fromStdString("HalfEdge " + std::to_string(id));
    this->setText(s);
}

HalfEdge::HalfEdge(Face *f, int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{

    face = f;
    this->id = id;
    QString s = QString::fromStdString("HalfEdge " + std::to_string(id));
    this->setText(s);
}

HalfEdge::HalfEdge(Vertex *v, int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{

    vert = v;
    this->id = id;
    QString s = QString::fromStdString("HalfEdge " + std::to_string(id));
    this->setText(s);
}

HalfEdge::HalfEdge(Vertex *v, Face *f, int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    //this->addChild(v);
    //this->addChild(f);
    vert = v;
    face = f;
    this->id = id;
    QString s = QString::fromStdString("HalfEdge " + std::to_string(id));
    this->setText(s);
}

void HalfEdge::pair(HalfEdge *s)
{
    sym = s;
    s->sym = this;
}

HalfEdge::~HalfEdge()
{
    this->destroy();
}

void HalfEdge::create()
{
    if (vert == NULL) {
        return;
    }
    if (sym == NULL) {
        return;
    }
    if (sym->vert == NULL) {
        return;
    }

    glm::vec4 edge_vert_pos[2];
    glm::vec4 edge_vert_col[2];
    GLuint edge_idx[2];

    edge_vert_pos[0] = vert->pos;
    edge_vert_pos[1] = sym->vert->pos;

    edge_vert_col[0] = glm::vec4(0.8f, 1, 0.9f, 0);
    edge_vert_col[1] = glm::vec4(0.8f, 1, 0.9f, 0);

    edge_idx[0] = 0;
    edge_idx[1] = 1;

    count = 2;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(edge_idx, 2 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(edge_vert_pos, 2 * sizeof(glm::vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(edge_vert_col, 2 * sizeof(glm::vec4));
}

void HalfEdge::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
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
    return 1;
}

bool HalfEdge::bindCol()
{
    return bufCol.bind();
}

