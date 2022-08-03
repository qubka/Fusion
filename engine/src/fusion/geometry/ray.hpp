#pragma once

namespace fe {
    class Ray {
    public:
        Ray();
        Ray(const glm::vec3& origin, const glm::vec3& direction);

        //! The origin point of the ray.
        const glm::vec3& getOrigin() const { return origin; }
        //! The direction of the ray.
        const glm::vec3& getDirection() const  { return direction; }
        //! Returns a point at /distance/ units along the ray.
        glm::vec3 getPoint(float distance) const  { return origin + direction * distance; }

        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        void transform(const glm::mat4& transform);
        //! Converts sphere to another coordinate system. Note that it will not return correct results if there are non-uniform scaling, shears, or other unusual transforms in \a transform.
        Ray transformed(const glm::mat4& transform) const;

        //! Returns true if triangle is within ray.
        bool triangleIntersection(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& result) const;
        //! Returns true if plane is within ray.
        bool planeIntersection(const glm::vec3& planeOrigin, const glm::vec3& normal, float& result) const;

    private:
        glm::vec3 origin;
        glm::vec3 direction;
    };
}
