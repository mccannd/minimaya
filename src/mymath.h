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
