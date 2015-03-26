# pragma once

#ifndef FACE_H
#define FACE_H
#include "la.h"
#include "QListWidgetItem"

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

    int getID();
};

#endif // FACE_H
