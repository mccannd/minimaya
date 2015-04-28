#include "face.h"

Face::Face(int id)
{
    this->id = id;
    color = glm::vec4(0.5, 0.5, 0.5, 0);
    QString s = QString::fromStdString("Face " + std::to_string(id));
    this->setText(s);
    mat_attr.rflec = 0.0f;
    mat_attr.alpha = 1.0f;
    mat_attr.rfrac = 1.0f;
    mat_attr.specl = 20.0f;
}

Face::Face(glm::vec4 c, int id)
{
    color = c;
    this->id = id;
    QString s = QString::fromStdString("Face " + std::to_string(id));
    this->setText(s);
    mat_attr.rflec = 0.0f;
    mat_attr.alpha = 1.0f;
    mat_attr.rfrac = 1.0f;
    mat_attr.specl = 20.0f;
}

Face::~Face()
{

}

int Face::getID()
{
    return id;
}

