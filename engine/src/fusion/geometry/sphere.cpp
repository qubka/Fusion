#include "sphere.hpp"
#include "aabb.hpp"
#include "ray.hpp"

using namespace fe;

#define EPSILON_VALUE 4.37114e-05

Sphere::Sphere() : center{0.0f}, radius{0.0f} {
}

Sphere::Sphere(const glm::vec3& center, float radius) : center{center}, radius{radius} {
    assert(radius >= 0 && "Radius cannot be negative");
}

bool Sphere::intersects(const AABB& box) const {
    return box.intersects(*this);
}

bool Sphere::intersects(const Ray& ray) const {
    float t;
    glm::vec3 temp {ray.getOrigin() - center};
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2 * glm::dot(temp, ray.getDirection());
    float c = glm::dot(temp, temp) - radius * radius;
    float disc = b * b - 4 * a * c;

    if (disc < 0) {
        return false;
    } else {
        float e = glm::sqrt(disc);
        float denom = 2 * a;
        t = (-b - e) / denom;

        if (t > EPSILON_VALUE) {
            return true;
        }

        t = (-b + e) / denom;
        if (t > EPSILON_VALUE) {
            return true;
        }
    }

    return false;
}

int Sphere::intersect(const Ray& ray, float& intersection) const {
    float t;
    glm::vec3 temp {ray.getOrigin() - center};
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2 * glm::dot(temp, ray.getDirection());
    float c = glm::dot(temp, temp) - radius * radius;
    float disc = b * b - 4 * a * c;

    if (disc < 0) {
        return 0;
    } else {
        float e = glm::sqrt(disc);
        float denom = 2 * a;
        t = (-b - e) / denom;

        if (t > EPSILON_VALUE) {
            intersection = t;
            return 1;
        }

        t = (-b + e) / denom;
        if (t > EPSILON_VALUE) {
            intersection = t;
            return 1;
        }
    }

    return 0;
}

int Sphere::intersect(const Ray& ray, float& min, float& max) const {
    glm::vec3 temp {ray.getOrigin() - center};
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2 * glm::dot(temp, ray.getDirection());
    float c = glm::dot(temp, temp) - radius * radius;
    float disc = b * b - 4 * a * c;

    int count = 0;
    if (disc >= 0) {
        float t;

        float e = glm::sqrt(disc);
        float denom = 2 * a;

        t = (-b - e) / denom;
        if (t > EPSILON_VALUE) {
            min = t;
            count++;
        }

        t = (-b + e) / denom;
        if (t > EPSILON_VALUE) {
            max = t;
            count++;
        }
    }

    return count;
}

glm::vec3 Sphere::closestPoint(const Ray& ray) const {
    float t;
    glm::vec3 diff {ray.getOrigin() - center};
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2 * glm::dot(diff, ray.getDirection());
    float c = glm::dot(diff, diff) - radius * radius;
    float disc = b * b - 4 * a * c;

    if (disc > 0) {
        float e = std::sqrt(disc);
        float denominator = 2 * a;
        t = (-b - e) / denominator;

        if (t > EPSILON_VALUE)
            return ray.getPoint(t);

        t = (-b + e) / denominator;
        if (t > EPSILON_VALUE)
            return ray.getPoint(t);
    }

    t = glm::dot(-diff, glm::normalize(ray.getDirection()));
    glm::vec3 onRay {ray.getPoint(t)};
    return center + glm::normalize(onRay - center) * radius;
}

void Sphere::transform(const glm::mat4& transform) {
    center = transform * glm::vec4{center, 1}; // vec4 -> vec3
    radius = glm::length(transform * glm::vec4{radius, 0, 0, 0});
}

Sphere Sphere::transformed(const glm::mat4& transform) const {
    return {
        transform * glm::vec4{center, 1}, // vec4 -> vec3
        glm::length(transform * glm::vec4{radius, 0, 0, 0})
    };
}

Sphere Sphere::CalculateBoundingSphere(const std::vector<glm::vec3>& points) {
    return CalculateBoundingSphere(points.data(), points.size());
}

Sphere Sphere::CalculateBoundingSphere(const glm::vec3* points, size_t size) {
    if (!size)
        return {};

    // compute minimal and maximal bounds
    glm::vec3 min(points[0]), max(points[0]);
    for (size_t i = 1; i < size; ++i) {
        if (points[i].x < min.x)
            min.x = points[i].x;
        else if (points[i].x > max.x)
            max.x = points[i].x;
        if (points[i].y < min.y)
            min.y = points[i].y;
        else if (points[i].y > max.y)
            max.y = points[i].y;
        if (points[i].z < min.z)
            min.z = points[i].z;
        else if (points[i].z > max.z)
            max.z = points[i].z;
    }
    // compute center and radius
    glm::vec3 center {0.5f * (min + max)};
    float maxDistance = glm::distance2(center, points[0]);
    for (size_t i = 1; i < size; ++i) {
        float dist = glm::distance2(center, points[i]);
        if (dist > maxDistance)
            maxDistance = dist;
    }

    return { center, std::sqrt(maxDistance) };
}

std::ostream& operator<<(std::ostream& o, const Sphere& s) {
    return o << "(" << glm::to_string(s.getCenter()) << ", " << s.getRadius() << ")";
}