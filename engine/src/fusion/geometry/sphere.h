#pragma once

namespace fe {
    class AABB;
    class Ray;

    class Sphere {
    public:
        Sphere();
        Sphere(const glm::vec3& center, float radius);

        //! Radius of the sphere.
        float getRadius() const { return radius; }
        //! Center of the sphere.
        const glm::vec3& getCenter() const { return center; }

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
        static Sphere CalculateBoundingSphere(gsl::span<const glm::vec3> points);

        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        void transform(const glm::mat4& transform);
        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        Sphere transformed(const glm::mat4& transform) const;

        //! Calculates the projection of the Sphere (an oriented ellipse) given \a focalLength. Returns \c false if calculation failed, rendering only \a outCenter correct. Algorithm due to Iñigo Quilez.
        void calcProjection(float focalLength, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const;
        //! Calculates the projection of the Sphere (an oriented ellipse) given \a focalLength. Algorithm due to Iñigo Quilez.
        void calcProjection(float focalLength, const glm::vec2& screenSize, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const;
        //! Calculates the projected area of the Sphere given \a focalLength and screen size in pixels. Algorithm due to Iñigo Quilez.
        float calcProjectedArea(float focalLength, const glm::vec2& screenSize) const;

    private:
        glm::vec3 center;
        float radius;
    };
}