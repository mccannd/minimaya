#pragma once
#include "la.h"
#include <iostream>

using namespace glm;

class mymath
{
public:
    mymath();
};

/// Prints the vector to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const vec4 &v);
int combination(int n, int r);
int factorial(int n);
vec4 cross(vec4 v1, vec4 v2);
