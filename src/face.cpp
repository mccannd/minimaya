#include "face.h"

Face::Face(int id)
{
    this->id = id;
    color = glm::vec4(0.5, 0.5, 0.5, 1);
    QString s = QString::fromStdString("Face " + std::to_string(id));
    this->setText(s);
}

Face::Face(glm::vec4 c, int id)
{
    color = c;
    this->id = id;
    QString s = QString::fromStdString("Face " + std::to_string(id));
    this->setText(s);
}

Face::~Face()
{

}

int Face::getID()
{
    return id;
}

