#pragma once

#include "la.h"
#include "QListWidgetItem"


#include "material.h"


class HalfEdge;
class Face : public QListWidgetItem
{
private:
    int id; // unique id
public:
    Face(int id);
    Face(glm::vec4 c, int id);
    ~Face();

    glm::vec4 color;
    HalfEdge* start_edge;
    Material mat_attr = Material::RUBBER;

    glm::vec4 norm = glm::vec4(0, 0, 0, -1);

    int getID();
};

