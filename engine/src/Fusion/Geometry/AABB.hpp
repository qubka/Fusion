#pragma once

#include "Fusion/Core/Base.hpp"

namespace Fusion {
    class Ray;
    class Sphere;

    class FUSION_API AABB {
        glm::vec3 center;
        glm::vec3 extents;
    public:
        AABB();
        AABB(const glm::vec3& min, const glm::vec3& max);

        //! Returns the center of the axis-aligned box.
        const glm::vec3& getCenter() const { return center; };
        //! Returns the extents of the axis-aligned box.
        const glm::vec3& getExtents() const { return extents; };
        //! Returns the size of the axis-aligned box.
        glm::vec3 getSize() const { return 2.0f * extents; };
        //! Returns the corner of the axis-aligned box with the smallest x, y and z coordinates.
        glm::vec3 getMin() const { return center - extents; };
        //! Returns the corner of the axis-aligned box with the largest x, y and z coordinates.
        glm::vec3 getMax() const { return center + extents; };

        //! Construct an axis-aligned box by specifying two opposite corners.
        void set(const glm::vec3& min, const glm::vec3& max);

        //! Expands the box so that it contains \a point.
        void include(const glm::vec3& point);
        //! Expands the box so that it contains \a box.
        void include(const AABB& box);
        //! Returns \c true if the axis-aligned box contains \a point.
        bool contains(const glm::vec3& point) const;
        //! Returns \c true if the axis-aligned boxes intersect.
        bool intersects(const AABB& box) const;
        //! Returns \c true if the axis-aligned box intersects \a sphere.
        bool intersects(const Sphere& sphere) const;
        //! Returns \c true if the ray intersects the axis-aligned box.
        bool intersects(const Ray& ray) const;
        //! Performs ray intersections and returns the number of intersections (0, 1 or 2). Returns \a min and \a max distance from the ray origin.
        int intersect(const Ray& ray, float& min, float& max) const;
        //! Given a plane through the origin with \a normal, returns the minimum and maximum distance to the axis-aligned box.
        void project(const glm::vec3& normal, float& min, float& max) const;

        //! Given a plane through the origin with \a normal, returns the corner closest to the plane.
        glm::vec3 getNegative(const glm::vec3& normal) const;
        //! Given a plane through the origin with \a normal, returns the corner farthest from the plane.
        glm::vec3 getPositive(const glm::vec3& normal) const;
        //! Calculates the distance to the axis-aligned box nearest point.
        float getDistanceToNearestEdge(const glm::vec3& point) const;

        //! Converts axis-aligned box to another coordinate space.
        void transform(const glm::mat4& transform);
        //! Converts axis-aligned box to another coordinate space.
        AABB transformed(const glm::mat4& transform) const;
    };
}

std::ostream& operator<<(std::ostream& o, const Fusion::AABB& b);