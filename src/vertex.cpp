#include "vertex.h"

Vertex::Vertex(int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    this->id = id;
    this->pos = glm::vec4(0, 0, 0, 1);
    QString s = QString::fromStdString("Vertex " + std::to_string(id));
    this->setText(s);
}

Vertex::Vertex(float x, float y, float z, int id)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    this->id = id;
    this->pos = glm::vec4(x, y, z, 1);
    QString s = QString::fromStdString("Vertex " + std::to_string(id));
    this->setText(s);
}


Vertex::~Vertex()
{

}

void Vertex::create()
{


    glm::vec4 vert_pos[1];
    glm::vec4 vert_col[1];
    GLuint vert_idx[1];

    vert_pos[0] = pos;

    vert_col[0] = glm::vec4(0.8f, 1, 0.9f, 0);

    vert_idx[0] = 0;

    count = 1;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(vert_idx, 1 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(vert_pos, 1 * sizeof(glm::vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(vert_col, 1 * sizeof(glm::vec4));
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
    return 1;
}

bool Vertex::bindCol()
{
    return bufCol.bind();
}

int Vertex::getID()
{
    return id;
}
