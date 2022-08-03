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

void Sphere::calcProjection(float focalLength, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const {
    glm::vec3 o {-center.x, center.y, center.z};

    float r2 = radius * radius;
    float z2 = o.z * o.z;
    float l2 = glm::dot(o, o);

    if (outCenter)
        *outCenter = focalLength * o.z * glm::vec2{o.x, o.y} / (z2 - r2);
    if (std::fabs(z2 - l2) > 0.00001f) {
        if (outAxisA)
            *outAxisA = focalLength * sqrtf(-r2 * (r2 - l2) / ((l2 - z2) * (r2 - z2) * (r2 - z2))) * glm::vec2{o.x, o.y};
        if (outAxisB)
            *outAxisB = focalLength * sqrtf(std::fabs(-r2 * (r2 - l2) / ((l2 - z2) * (r2 - z2) * (r2 - l2)))) * glm::vec2{-o.y, o.x};
    } else { // approximate with circle
        float rad = focalLength * radius / sqrtf(z2 - r2);
        if (outAxisA)
            *outAxisA = {rad, 0};
        if (outAxisB)
            *outAxisB = {0, rad};
    }
}

void Sphere::calcProjection(float focalLength, const glm::vec2& screenSize, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return;

    auto toScreenPixels = [=](glm::vec2 result, const glm::vec2& windowSize) {
        result.x *= 1 / (windowSize.x / windowSize.y);
        result += glm::vec2{0.5f};
        result *= windowSize;
        return result;
    };

    glm::vec2 centerO, axisA, axisB;
    calcProjection(focalLength, &centerO, &axisA, &axisB);

    if (outCenter)
        *outCenter = toScreenPixels(centerO, screenSize);

    if (outAxisA)
        *outAxisA = toScreenPixels(centerO + axisA * 0.5f, screenSize) -
                    toScreenPixels(centerO - axisA * 0.5f, screenSize);
    if (outAxisB)
        *outAxisB = toScreenPixels(centerO + axisB * 0.5f, screenSize) -
                    toScreenPixels(centerO - axisB * 0.5f, screenSize);
}

float Sphere::calcProjectedArea(float focalLength, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return 0.0f;

    glm::vec3 o {center};

    float r2 = radius * radius;
    float z2 = o.z * o.z;
    float l2 = glm::dot(o, o);

    float area = static_cast<float>(-M_PI) * focalLength * focalLength * r2 * std::sqrt(std::fabs((l2 - r2) / (r2 - z2))) / (r2 - z2);
    float aspectRatio = screenSize.x / screenSize.y;
    return area * screenSize.x * screenSize.y * 0.25f / aspectRatio;
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

Sphere Sphere::CalculateBoundingSphere(std::span<const glm::vec3> points) {
    if (points.empty())
        return {};

    // compute minimal and maximal bounds
    glm::vec3 min{points[0]}, max{points[0]};
    for (size_t i = 1; i < points.size(); ++i) {
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
    for (size_t i = 1; i < points.size(); ++i) {
        float dist = glm::distance2(center, points[i]);
        if (dist > maxDistance)
            maxDistance = dist;
    }

    return { center, std::sqrt(maxDistance) };
}