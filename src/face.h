#pragma once

#include "la.h"
#include "QListWidgetItem"


typedef struct {
  float rflec, alpha, rfrac, specl;
} Material;

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
    Material mat_attr;

    glm::vec4 norm = glm::vec4(0, 0, 0, -1);

    int getID();
};

