#include "frustum.hpp"
#include "aabb.hpp"
#include "sphere.hpp"

using namespace fe;

Frustum::Frustum(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr) {
    set(ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr);
}

Frustum::Frustum(const glm::mat4& mat) {
    set(mat);
}

void Frustum::set(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr) {
    planes[TOP].set(ntr, ntl, ftl);
    planes[BOTTOM].set(nbl, nbr, fbr);
    planes[LEFT].set(ntl, nbl, fbl);
    planes[RIGHT].set(nbr, ntr, fbr);
    planes[NEAR].set(ntl, ntr, nbr);
    planes[FAR].set(ftr, ftl, fbl);
}

void Frustum::set(const glm::mat4& mat) {
    // Based on: Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix
    //       by: Gil Gribb and Klaus Hartmann
    /// @link https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    planes[TOP].set({mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], mat[3][3] - mat[3][1]});
    planes[BOTTOM].set({mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], mat[3][3] + mat[3][1]});
    planes[LEFT].set({mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], mat[3][3] + mat[3][0]});
    planes[RIGHT].set({mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], mat[3][3] - mat[3][0]});
    planes[NEAR].set({mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], mat[3][3] + mat[3][2]});
    planes[FAR].set({mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], mat[3][3] - mat[3][2]});
}

bool Frustum::contains(const glm::vec3& loc) const {
    for (auto plane : planes) {
        if (plane.getDistanceToPoint(loc) < 0)
            return false;
    }
    return true;
}

bool Frustum::contains(const glm::vec3& center, float radius) const {
    float distance;
    for (auto plane : planes) {
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
    for (auto plane : planes) {
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
    for (auto plane : planes) {
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
    for (auto plane : planes) {
        if (plane.getDistanceToPoint(box.getPositive(plane.getNormal())) < 0)
            return false;
    }

    return true;
}