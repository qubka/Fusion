#include "ray.h"

using namespace fe;

Ray::Ray() : origin{0.0f}, direction{0.0f} {
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin{origin}, direction{direction} {
}

///@brief algorithm from "Fast, Minimum Storage Ray-Triangle Intersection"
bool Ray::triangleIntersection(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& result) const {
    glm::vec3 e1 {v1 - v0};
    glm::vec3 e2 {v2 - v0};

    glm::vec3 p {glm::cross(direction, e2)};
    float det = glm::dot(e1, p);

#if 0 // we don't want to backface cull
    if (det < FLT_EPSILON)
		  return false;

    glm::vec3 t {origin - v0};

	float u = glm::dot(t, p);
	if (u < 0 || u > det)
		return false;

    glm::vec3 q {glm::cross(t, e1)};
	float v = glm::dot(direction, q);
	if (v < 0 || u + v > det)
		return false;

	result = glm::dot(e2, q) / det;
	return true;
#else
    if (det > -FLT_EPSILON && det < FLT_EPSILON)
        return false;

    glm::vec3 t {origin - v0};

    float invDet = 1 / det;

    float u = glm::dot(t, p) * invDet;
    if (u < 0 || u > 1)
        return false;

    glm::vec3 q {glm::cross(t, e1)};

    float v = glm::dot(direction, q) * invDet;
    if (v < 0 || u + v > 1)
        return false;

    result = glm::dot(e2, q) * invDet;
    return true;
#endif
}

bool Ray::planeIntersection(const glm::vec3& planeOrigin, const glm::vec3& planeNormal, float& result) const {
    float denominator = glm::dot(planeNormal, direction);

    if (denominator != 0) {
        result = glm::dot(planeNormal, planeOrigin - origin) / denominator;
        return true;
    }

    return false;
}

void Ray::transform(const glm::mat4& transform) {
    origin = transform * glm::vec4{origin, 1}; // vec4 -> vec3
    direction =  glm::mat3{transform} * direction;
}

Ray Ray::transformed(const glm::mat4& transform) const {
    return {
        transform * glm::vec4{origin, 1}, // vec4 -> vec3
        glm::mat3{transform} * direction
    };
}