#pragma once

namespace Fusion {
    class Ray;

    class Plane {
        glm::vec3 normal;
        float distance;

    public:
        Plane();
        Plane(const glm::vec3& normal, const glm::vec3& point);
        Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        Plane(const glm::vec3& normal, float distance);
        explicit Plane(const glm::vec4& abcd);

        //! Defines a plane using a normal vector and a point.
        void set(const glm::vec3& normal, const glm::vec3& point);
        //! Defines a plane using 3 points.
        void set(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        //! Defines a plane using 4 coefficients.
        void set(const glm::vec4& abcd);
        //! Defines a plane using a normal vector and a distance.
        void set(const glm::vec3& normal, float distance);

        //! Point from the plane
        glm::vec3 getPoint() const { return normal * distance; }
        //! Normal vector of the plane.
        const glm::vec3& getNormal() const { return normal; }
        //! Distance from the origin to the plane.
        float getDistance() const { return distance; }

        //! Translates the plane into a given direction.
        void translate(const glm::vec3& translation);
        //! Creates a plane that's translated into a given direction.
        Plane translated(const glm::vec3& translation) const;

        //! Make the plane face the opposite direction.
        void flip();
        //! Return a version of the plane that faces the opposite direction.
        Plane flipped() const;

        //! Calculates the reflected point on the plane.
        glm::vec3 reflectPoint(const glm::vec3& point) const;
        //! Calculates the reflected vector on the plane.
        glm::vec3 reflectVector(const glm::vec3& vector) const;
        //! Calculates the closest point on the plane.
        glm::vec3 closestPoint(const glm::vec3& point) const;
        //! Returns a signed distance from plane to point.
        float getDistanceToPoint(const glm::vec3& point) const;
        //! Are two points on the same side of the plane?
        bool sameSide(const glm::vec3& p0, const glm::vec3& p1) const;
        //! Intersects a ray with the plane.
        bool rayCast(const Ray& ray, float& enter) const;
    };
}

std::ostream& operator<<(std::ostream& o, const Fusion::Plane& p);