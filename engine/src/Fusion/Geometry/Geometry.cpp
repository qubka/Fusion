#include "Geometry.hpp"
#include "Plane.hpp"

using namespace Fusion;

glm::vec3 Geometry::intersectionPoint(const Plane& a, const Plane& b, const Plane& c) {
    float f = -glm::dot(a.getNormal(), glm::cross(b.getNormal(), c.getNormal()));

    glm::vec3 v1 = a.getDistance() * glm::cross(b.getNormal(), c.getNormal());
    glm::vec3 v2 = b.getDistance() * glm::cross(c.getNormal(), a.getNormal());
    glm::vec3 v3 = c.getDistance() * glm::cross(a.getNormal(), b.getNormal());

    return (v1 + v2 + v3) / f;
}