#include "mymath.h"
#include "float.h"

mymath::mymath()
{
}

// Prints the vector to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const vec4 &v) {
    o << "{" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "}";
    return o;
}

// n choose r
int combination(int n, int r) {
    return factorial(n) / (factorial(r) * factorial(n - r));
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

vec4 cross(vec4 v1, vec4 v2) {
    vec3 v_1 = vec3(v1[0], v1[1], v1[2]);
    vec3 v_2 = vec3(v2[0], v2[1], v2[2]);
    vec3 result = glm::cross(v_1, v_2);
    return vec4(result[0], result[1], result[2], 0);
}
