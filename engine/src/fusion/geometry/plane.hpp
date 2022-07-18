#pragma once

namespace fe {
    class Ray;

    class Plane {
    public:
        Plane();
        Plane(const glm::vec3& normal, const glm::vec3& point);
        Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        Plane(const glm::vec3& normal, float distance);
        Plane(float a, float b, float c, float d);
        explicit Plane(const glm::vec4& abcd);

        //! Defines a plane using a normal vector and a point.
        void set(const glm::vec3& normal, const glm::vec3& point);
        //! Defines a plane using 3 points.
        void set(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        //! Defines a plane using 4 coefficients.
        void set(const glm::vec4& abcd);
        //! Defines a plane using 4 coefficients.
        void set(float a, float b, float c, float d);
        //! Defines a plane using a normal vector and a distance.
        void set(const glm::vec3& normal, float distance);

        //! Point from the plane
        glm::vec3 getPoint() const { return normal * distance; }
        //! Normal vector of the plane.
        const glm::vec3& getNormal() const { return normal; }
        //! Sets the normal vector of the plane.
        void setNormal(const glm::vec3& normal);
        //! Distance from the origin to the plane.
        float getDistance() const { return distance; }
        //! Sets the dictance from the origin to the plane.
        void setDistance(float distance);

        //! Translates the plane into a given direction.
        void translate(const glm::vec3& translation);
        //! Creates a plane that's translated into a given direction.
        Plane translated(const glm::vec3& translation) const;
        //! Transform the plane by a given transform.
        void transform(const glm::mat4& transform);
        //! Creates a plane that's transformed into a given transform.
        Plane transformed(const glm::mat4& transform) const;

        //! Normalize the plane.
        void normalize();
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
        //! Are two points on the same side of the plane?
        bool sameSide(const glm::vec3& p0, const glm::vec3& p1) const;
        //! Intersects a ray with the plane.
        bool rayCast(const Ray& ray, float& enter) const;
        //! Returns a signed distance from plane to point.
        float getDistanceToPoint(const glm::vec3& point) const;
        //! Is point on the plane?
        bool isPointOnPlane(const glm::vec3& point) const;

    private:
        glm::vec3 normal;
        float distance;
    };
}

std::ostream& operator<<(std::ostream& o, const fe::Plane& p);