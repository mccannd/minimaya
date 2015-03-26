#include "vertex.h"

Vertex::Vertex(int id)
{
    this->id = id;
    this->pos = glm::vec4(0, 0, 0, 1);
    QString s = QString::fromStdString("Vertex " + std::to_string(id));
    this->setText(s);
}

Vertex::Vertex(float x, float y, float z, int id)
{
    this->id = id;
    this->pos = glm::vec4(x, y, z, 1);
    QString s = QString::fromStdString("Vertex " + std::to_string(id));
    this->setText(s);
}


Vertex::~Vertex()
{

}

