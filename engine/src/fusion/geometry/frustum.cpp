#include "frustum.hpp"
#include "aabb.hpp"
#include "sphere.hpp"
#include "rect.hpp"

using namespace fe;

Frustum::Frustum() {
    set(glm::mat4{1.0f});
}
Frustum::Frustum(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr) {
    set(ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr);
}

Frustum::Frustum(const glm::mat4& m) {
    set(m);
}

void Frustum::set(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr) {
    planes[Top].set(ntr, ntl, ftl);
    planes[Bottom].set(nbl, nbr, fbr);
    planes[Left].set(ntl, nbl, fbl);
    planes[Right].set(nbr, ntr, fbr);
    planes[Near].set(ntl, ntr, nbr);
    planes[Far].set(ftr, ftl, fbl);

    //calculateVertices(m);
}

void Frustum::set(const glm::mat4& m) {
    // Based on: Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix
    //       by: Gil Gribb and Klaus Hartmann
    /// @link https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    planes[Top].set({m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]});
    planes[Bottom].set({m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]});
    planes[Left].set({m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]});
    planes[Right].set({m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]});
    planes[Near].set({m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]});
    planes[Far].set({m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]});

    calculateVertices(m);
}

void Frustum::transform(const glm::mat4& m) {
    for (auto& plane : planes) {
        plane.transform(m);
        plane.normalize();
    }

    calculateVertices(m);
}

bool Frustum::contains(const glm::vec3& loc) const {
    for (const auto& plane : planes) {
        if (plane.getDistanceToPoint(loc) < 0)
            return false;
    }
    return true;
}

bool Frustum::contains(const glm::vec3& center, float radius) const {
    float distance;
    for (const auto& plane : planes) {
        distance = plane.getDistanceToPoint(center);
        if (distance < radius)
            return false;
    }
    return true;
}

bool Frustum::contains(const glm::vec3& center, const glm::vec3& size) const {
    glm::vec3 halfSize{ size * 0.5f };
    AABB box{center - halfSize, center + halfSize};
    return contains(box);
}

bool Frustum::contains(const Sphere& sphere) const {
    return contains(sphere.getCenter(), sphere.getRadius());
}

bool Frustum::contains(const AABB& box) const {
    for (const auto& plane : planes) {
        if (plane.getDistanceToPoint(box.getPositive(plane.getNormal())) < 0 ||
            plane.getDistanceToPoint(box.getNegative(plane.getNormal())) < 0)
            return false;
    }
    return true;
}

bool Frustum::intersects(const glm::vec3& loc) const {
    return contains(loc);
}

bool Frustum::intersects(const glm::vec3& center, float radius) const {
    float distance;
    for (const auto& plane : planes) {
        distance = plane.getDistanceToPoint(center);
        if (distance < -radius)
            return false;
    }
    return true;
}

bool Frustum::intersects(const glm::vec3& center, const glm::vec3& size) const {
    glm::vec3 halfSize{ size * 0.5f };
    AABB box{center - halfSize, center + halfSize};
    return intersects(box);
}

bool Frustum::intersects(const Sphere& sphere) const {
    return intersects(sphere.getCenter(), sphere.getRadius());
}

bool Frustum::intersects(const AABB& box) const {
    for (const auto& plane : planes) {
        if (plane.getDistanceToPoint(box.getPositive(plane.getNormal())) < 0)
            return false;
    }
    return true;
}

void Frustum::calculateVertices(const glm::mat4& transform) {
    glm::mat4 transformInv{ glm::inverse(transform) };
    vertices[0] = glm::vec3{-1.0f, -1.0f, -1.0f};
    vertices[1] = glm::vec3{1.0f, -1.0f, -1.0f};
    vertices[2] = glm::vec3{1.0f, 1.0f, -1.0f};
    vertices[3] = glm::vec3{-1.0f, 1.0f, -1.0f};

    vertices[4] = glm::vec3{-1.0f, -1.0f, 1.0f};
    vertices[5] = glm::vec3{1.0f, -1.0f, 1.0f};
    vertices[6] = glm::vec3{1.0f, 1.0f, 1.0f};
    vertices[7] = glm::vec3{-1.0f, 1.0f, 1.0f};

    for (auto& vertex : vertices) {
        glm::vec4 ndc{ transformInv * glm::vec4{vertex, 1} };
        vertex = ndc / ndc.w;
    }
}

