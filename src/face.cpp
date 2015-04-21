#include "face.h"

Face::Face() : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    QListWidgetItem::setText(QString("Not"));
}

Face::Face(int i) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer)
{
    id = i;
    QListWidgetItem::setText(QString::number(id));
}

Face::Face(int i, vec4 c, HalfEdge* s) : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    id = i;
    color = c;
    start_edge = s;
    QListWidgetItem::setText(QString::number(id));
}

void Face::create()
{
    vector<vec4> face_vert_pos = {};
    vector<vec4> face_vert_col = {};
    vector<GLuint> face_idx = {};

    HalfEdge* curr = start_edge;
    do {
        face_vert_pos.push_back(curr->vert->pos);
        face_vert_col.push_back(vec4(0,1,0,1));
        curr = curr->next;
    } while (curr != start_edge);

    for(std::vector<vec4>::size_type i = 0; i < face_vert_pos.size() - 1; i++) {
        face_idx.push_back(i);
        face_idx.push_back(i + 1);
    }
    face_idx.push_back(face_vert_pos.size() - 1);
    face_idx.push_back(0);

    count = face_idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(face_idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(face_vert_pos.data(), face_vert_pos.size() * sizeof(vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(face_vert_col.data(), face_vert_col.size() * sizeof(vec4));
}

void Face::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufCol.destroy();
}

GLenum Face::drawMode()
{
    return GL_LINES;
}

int Face::elemCount()
{
    return count;
}

bool Face::bindIdx()
{
    return bufIdx.bind();
}

bool Face::bindPos()
{
    return bufPos.bind();
}

bool Face::bindNor()
{
    return bufNor.bind();
}

bool Face::bindCol()
{
    return bufCol.bind();
}
