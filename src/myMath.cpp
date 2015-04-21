#include "myMath.h"
#include "float.h"

mymath::mymath()
{
}


vec4 cross(vec4 v1, vec4 v2) {
    vec3 v_1 = vec3(v1[0], v1[1], v1[2]);
    vec3 v_2 = vec3(v2[0], v2[1], v2[2]);
    vec3 result = glm::cross(v_1, v_2);
    return vec4(result[0], result[1], result[2], 0);
}


///  Coplanar: (a x b) * c <= tolerance
bool planarity(Face* f, float tolerance) {
    if (f == NULL) {
        return true;
    }

    HalfEdge* curr = f->start_edge;

    // Normal
    cout << curr->vert->pos;
    cout << curr->sym->vert->pos;
    cout << curr->next->vert->pos;
    cout << curr->vert->pos;
    cout << "\n\n";
    vec4 nor = cross(curr->vert->pos - curr->sym->vert->pos,
                     curr->next->vert->pos - curr->vert->pos);

    // Check if perpendicular to each edge
    do {
        float err = dot(nor, curr->vert->pos - curr->sym->vert->pos);

        // Check against tolerance
        if (abs(err) > tolerance) {
            return false;
        }

        curr = curr->next;
    } while (curr != f->start_edge);

    return true;
}

vec4 col_opp(vec4 c) {
    return vec4(1 - c[0], 1 - c[1], 1 - c[2], 1);
}

vec4 mid(HalfEdge* h) {
    vec4 a = h->sym->vert->pos;
    vec4 b = h->vert->pos;

    return vec4((b[0] + a[0])/2.0f, (b[1] + a[1])/2.0f, (b[2] + a[2])/2.0f, 1);
}

vec4 midpoint(HalfEdge* h) {
    float denom = 2.0;
    vec4 temp = h->sym->vert->pos + h->vert->pos;

    if (h->face != NULL) {
            temp += h->face->centroid->pos;
            denom++;
    }
    if (h->sym->face != NULL) {
            temp += h->sym->face->centroid->pos;
            denom++;
    }
    return temp / denom;
}

// Prints the vector to a stream in a nice format
std::ostream &operator<<(std::ostream &o, const vec4 &v) {
    o << "{" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "}";
    return o;
}

QString vec4toString(const vec4 &v) {
    std::string temp = "{" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + ", " +
            std::to_string(v[2]) + ", " + std::to_string(v[3]) + "}";
     return QString::fromStdString(temp);
}
