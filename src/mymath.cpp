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

// n choose i
int combination(int n, int i) {
    factorial(n) / (factorial(i) * factorial(n - i));
}

int factorial(int n) {
    if (n == 1) {
        return n;
    } else {
        return n * factorial(n - 1);
    }
}
