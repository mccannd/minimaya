//#include "interpolationutils.h"


//interpolationUtils::interpolationUtils()
//{

//}

//interpolationUtils::~interpolationUtils()
//{

//}

//glm::vec4 cubicInterpolation(const glm::vec4 &a,
//                             const glm::vec4 &b,
//                             const glm::vec4 &c,
//                             const glm::vec4 &d,
//                             double t)
//{
//    // de castlejau implementation
//    // find first midpoints
//    glm::vec4 m1 = (1 - t) * a + t * b;
//    glm::vec4 m2 = (1 - t) * b + t * c;
//    glm::vec4 m3 = (1 - t) * c + t * d;

//    // find second midpoints
//    glm::vec4 mm1 = (1 - t) * m1 + t * m2;
//    glm::vec4 mm2 = (1 - t) * m2 + t * m3;

//    return (1 - t) * mm1 + t * mm2;
//}

//void createInterpolatedKeys(const std::vector<std::pair<int, glm::quat>> &current,
//                            const std::vector<glm::quat> &target)
//{
//    // current vector has the keyframed keys
//    // target is going to contain only quaternion
//    target.clear();
//    for (unsigned int i = 1; i < current.size() - 1; i++) {
//        glm::quat currentQuat = current[i].second;
//        glm::quat prevQuat = current[i - 1].second;

//    }
//}

//std::pair<glm::vec4> colinearCtrlPts(const glm::vec4 &a,
//                                     const glm::vec4 &b,
//                                     const glm::vec4 &center)
//{
//    // create the slope for all dimensions
//    float dx = (b[0] - a[0]) / 6.0;
//    float dy = (b[1] - a[1]) / 6.0;
//    float dz = (b[2] - a[2]) / 6.0;

//    // we assume that a and b are keyframes before and after a target keyframe
//    // the keyframe is halfway between according to time

//    // thus the first position is 1/3 of the way across, 2ns is 2/3
//    glm::vec4 v1(center[0] - dx,
//                 center[1] - dy,
//                 center[2] - dz);
//    glm::vec4 v2(center[0] + dx,
//                 center[1] + dy,
//                 center[2] + dz);
//    return std::pair<glm::vec4>(v1, v2);
//}
