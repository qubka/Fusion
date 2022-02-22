#include "aabb.hpp"
#include "ray.hpp"
#include "sphere.hpp"

using namespace fe;

AABB::AABB() : center{0}, extents{0} {

}

AABB::AABB(const glm::vec3& min, const glm::vec3& max) {
    set(min, max);
}

void AABB::set(const glm::vec3& min, const glm::vec3& max) {
    center = 0.5f * (min + max);
    extents = glm::abs(max - center);
}

void AABB::include(const glm::vec3& point) {
    glm::vec3 min {glm::min(getMin(), point)};
    glm::vec3 max {glm::max(getMax(), point)};
    set(min, max);
}

void AABB::include(const AABB& box) {
    glm::vec3 min {glm::min(getMin(), box.getMin())};
    glm::vec3 max {glm::max(getMax(), box.getMax())};
    set(min, max);
}

bool AABB::contains(const glm::vec3& point) const {
    return glm::all(glm::lessThanEqual(glm::abs(point - center), extents));
}

bool AABB::intersects(const AABB& box) const {
    return glm::all(glm::lessThanEqual(glm::abs(box.center - center), box.extents + extents));
}

bool AABB::intersects(const Sphere& sphere) const {
    float dmin = 0;

    const auto& center = sphere.getCenter();
    glm::vec3 bmin {getMin()};
    glm::vec3 bmax {getMax()};

    if (center.x < bmin.x) {
        float d = center.x - bmin.x;
        dmin += d * d;
    } else if (center.x > bmax.x) {
        float d = center.x - bmax.x;
        dmin += d * d;
    }

    if (center.y < bmin.y) {
        float d = center.y - bmin.y;
        dmin += d * d;
    } else if (center.y > bmax.y) {
        float d = center.y - bmax.y;
        dmin += d * d;
    }

    if (center.z < bmin.z) {
        float d = center.z - bmin.z;
        dmin += d * d;
    } else if (center.z > bmax.z) {
        float d = center.z - bmax.z;
        dmin += d * d;
    }

    return dmin <= (sphere.getRadius() * sphere.getRadius());
}

bool AABB::intersects(const Ray& ray) const {
    if (glm::length2(ray.getDirection()) < FLT_EPSILON)
        return false;

    glm::vec3 min {(getMin() - ray.getOrigin()) / ray.getDirection()};
    glm::vec3 max {(getMax() - ray.getOrigin()) / ray.getDirection()};

    float fmin = glm::max(glm::max(glm::min(min.x, max.x), glm::min(min.y, max.y)), glm::min(min.z, max.z));
    float fmax = glm::min(glm::min(glm::max(min.x, max.x), glm::max(min.y, max.y)), glm::max(min.z, max.z));

    return (fmax >= fmin);
}

int AABB::intersect(const Ray& ray, float& min, float& max) const {
    if (glm::length2(ray.getDirection()) < FLT_EPSILON)
        return 0;

    glm::vec3 _min {(getMin() - ray.getOrigin()) / ray.getDirection()};
    glm::vec3 _max {(getMax() - ray.getOrigin()) / ray.getDirection()};

    float fmin = glm::max(glm::max(glm::min(_min.x, _max.x), glm::min(_min.y, _max.y)), glm::min(_min.z, _max.z));
    float fmax = glm::min(glm::min(glm::max(_min.x, _max.x), glm::max(_min.y, _max.y)), glm::max(_min.z, _max.z));

    if (fmax >= fmin) {
        min = fmin;
        max = fmax;

        if (fmax > fmin)
            return 2;
        else
            return 1;
    }

    return 0;
}

void AABB::project(const glm::vec3& normal, float& min, float& max) const {
    float p = glm::dot(normal, center);
    float d = glm::dot(glm::abs(normal), extents);
    min = p - d;
    max = p + d;
}

glm::vec3 AABB::getNegative(const glm::vec3& normal) const {
    glm::vec3 result {getMin()};
    glm::vec3 size {getSize()};

    if (normal.x < 0)
        result.x += size.x;

    if (normal.y < 0)
        result.y += size.y;

    if (normal.z < 0)
        result.z += size.z;

    return result;
}

glm::vec3 AABB::getPositive(const glm::vec3& normal) const {
    glm::vec3 result {getMin()};
    glm::vec3 size {getSize()};

    if (normal.x > 0)
        result.x += size.x;

    if (normal.y > 0)
        result.y += size.y;

    if (normal.z > 0)
        result.z += size.z;

    return result;
}

float AABB::getDistanceToNearestEdge(const glm::vec3& point) const {
    glm::vec3 min {getMin()};
    glm::vec3 max {getMax()};
    
    std::array<glm::vec3, 12> edges = {
        min,
        max,
        {min.x, min.y, max.z},
        {min.x, max.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, min.z},
        {min.x, center.y, max.z},
        {min.x, center.y, min.z},
        {max.x, center.y, min.z},
        {min.x, center.y, min.z}
    };

    float minDistance = glm::distance2(point, edges[0]);
    for (int i = 1; i < edges.size(); ++i) {
        float dist = glm::distance2(point, edges[i]);
        if (dist < minDistance)
            minDistance = dist;
    }

    return std::sqrt(minDistance);
}

void AABB::transform(const glm::mat4& transform) {
    glm::mat3 m{transform};
    glm::vec3 x {m * glm::vec3{extents.x, 0, 0}};
    glm::vec3 y {m * glm::vec3{0, extents.y, 0}};
    glm::vec3 z {m * glm::vec3{0, 0, extents.z}};

    extents = glm::abs(x) + glm::abs(y) + glm::abs(z);
    center = transform * glm::vec4{center, 1}; // vec4 -> vec3
}

AABB AABB::transformed(const glm::mat4& transform) const {
    glm::mat3 m{transform};
    glm::vec3 x {m * glm::vec3{extents.x, 0, 0}};
    glm::vec3 y {m * glm::vec3{0, extents.y, 0}};
    glm::vec3 z {m * glm::vec3{0, 0, extents.z}};

    return {
        glm::abs(x) + glm::abs(y) + glm::abs(z),
        transform * glm::vec4{center, 1} // vec4 -> vec3
    };
}

std::ostream& operator<<(std::ostream& o, const AABB& b) {
    return o << "(" << glm::to_string(b.getMin()) << ", " << glm::to_string(b.getMax()) << ")";
}