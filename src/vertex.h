#pragma once

#include <QListWidgetItem>
#include "halfedge.h"
#include "la.h"
class HalfEdge;
class Vertex : public QListWidgetItem
{
private:
    int id; // a unique id for user interface purposes
public:
    // constructors
    Vertex(int id);
    Vertex(float x, float y, float z, int id);
    ~Vertex();

    glm::vec4 pos; // location of vertex
    HalfEdge* edge;

    int getID();
};

