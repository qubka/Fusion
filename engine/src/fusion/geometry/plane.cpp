#include "plane.hpp"
#include "ray.hpp"

using namespace fe;

Plane::Plane() : normal{0}, distance{0} {
}

Plane::Plane(const glm::vec3& n, const glm::vec3& p) {
    set(n, p);
}

Plane::Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    set(a, b, c);
}

Plane::Plane(const glm::vec4& abcd) {
    set(abcd);
}

Plane::Plane(const glm::vec3& n, float d) {
    set(n, d);
}

void Plane::set(const glm::vec3& n, const glm::vec3& p) {
    if (glm::length2(n) == 0)
        throw std::invalid_argument("Degenerate сase exception");

    normal = glm::normalize(n);
    distance = -glm::dot(normal, p);
}

void Plane::set(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 n {glm::cross(b - a, c - a)};

    if (glm::length2(n) == 0)
        throw std::invalid_argument("Degenerate сase exception");

    normal = glm::normalize(n);
    distance = -glm::dot(normal, a);
}

void Plane::set(const glm::vec4& m) {
    glm::vec3 n{m};

    float length = glm::length2(n);
    if (length == 0)
        throw std::invalid_argument("Degenerate сase exception");

    normal = glm::normalize(n);
    distance = m.w / glm::sqrt(length);
}

void Plane::set(const glm::vec3& n, float d) {
    if (glm::length2(n) == 0)
        throw std::invalid_argument("Degenerate сase exception");

    normal = glm::normalize(n);
    distance = d;
}

void Plane::translate(const glm::vec3& translation) {
    distance += glm::dot(normal, translation);
}

Plane Plane::translated(const glm::vec3& translation) const {
    return { normal, distance + glm::dot(normal, translation) };
}

void Plane::flip() {
    normal = -normal;
    distance = -distance;
}

Plane Plane::flipped() const {
    return { -normal, -distance };
}

glm::vec3 Plane::closestPoint(const glm::vec3& point) const {
    return point - (normal * getDistanceToPoint(point));
}

float Plane::getDistanceToPoint(const glm::vec3& point) const {
    return glm::dot(normal, point) + distance;
}

bool Plane::sameSide(const glm::vec3& p0, const glm::vec3& p1) const {
    float d0 = getDistanceToPoint(p0);
    float d1 = getDistanceToPoint(p1);
    return (d0 >  0 && d1 >  0) ||
           (d0 <= 0 && d1 <= 0);
}

bool Plane::rayCast(const Ray& ray, float& enter) const {
    float vdot = glm::dot(ray.getDirection(), normal);

    if (std::abs(vdot) < FLT_EPSILON) {
        enter = 0;
        return false;
    }

    float ndot = -glm::dot(ray.getOrigin(), normal) - distance;

    enter = ndot / vdot;

    return enter > 0;
}

glm::vec3 Plane::reflectPoint(const glm::vec3& p) const {
    return normal * getDistanceToPoint(p) * -2.0f + p;
}

glm::vec3 Plane::reflectVector(const glm::vec3& v) const {
    return normal * glm::dot(normal, v) * 2.0f - v;
}

std::ostream& operator<<(std::ostream& o, const Plane& p) {
    return o << "(" << glm::to_string(p.getNormal()) << ", " << p.getDistance() << ")";
}