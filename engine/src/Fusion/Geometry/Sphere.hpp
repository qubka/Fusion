#pragma once

namespace Fusion {
    class AABB;
    class Ray;

    class FUSION_API Sphere {
        glm::vec3 center;
        float radius;
    public:
        Sphere();
        Sphere(const glm::vec3& center, float radius);

        //! Radius of the sphere.
        float getRadius() const { return radius; };
        //! Center of the sphere.
        const glm::vec3& getCenter() const { return center; };

        //! Returns \c true if the sphere intersects \a axis-aligned box.
        bool intersects(const AABB& box) const;
        //! Returns \c true if the sphere intersects \a ray.
        bool intersects(const Ray& ray) const;
        //! Returns \c intersection distance if the sphere intersects \a ray.
        int intersect(const Ray& ray, float& intersection) const;
        //! Returns \c intersection bounding box if the sphere intersects \a ray.
        int intersect(const Ray& ray, float& min, float& max) const;
        ///! Returns the closest point on \a ray to the Sphere. If \a ray intersects then returns the point of nearest intersection.
        glm::vec3 closestPoint(const Ray& ray) const;

        //! Generate \c sphere from the given sequence of \a points.
        static Sphere calculateBoundingSphere(const std::vector<glm::vec3>& points);
        //! Generate \c sphere from the given sequence of \a points.
        static Sphere calculateBoundingSphere(const glm::vec3* points, size_t size);

        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        void transform(const glm::mat4& transform);
        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        Sphere transformed(const glm::mat4& transform) const;

        static constexpr double EPSILON_VALUE = 4.37114e-05;
    };
}

std::ostream& operator<<(std::ostream& o, const Fusion::Sphere& s);