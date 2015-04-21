#pragma once

#include "la.h"
#include "face.h"
#include "halfedge.h"
#include "vertex.h"
#include <iostream>

using namespace glm;
class Face;
class HalfEdge;

class mymath
{
public:
    mymath();
};

vec4 cross(vec4 v1, vec4 v2);
bool planarity(Face* f, float tolerance);
vec4 col_opp(vec4 c);
vec4 mid(HalfEdge* h);
vec4 midpoint(HalfEdge* h);
QString vec4toString(const vec4 &v);

/// Prints the vector to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const vec4 &v);
