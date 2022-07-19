#pragma once

#include "plane.hpp"

namespace fe {
    class Sphere;
    class AABB;
    class Rect;
    /// @brief Create frustum from matrix
    /// if extracted from projection matrix only, planes will be in eye-space
    /// if extracted from projection*view, planes will be in world space
    /// if extracted from projection*view*model planes will be in model space
    class Frustum {
    public:
        enum FrustumSection { Top, Bottom, Left, Right, Near, Far };

        Frustum();
        //! Creates a frustum based on the corners of a near and far portal.
        Frustum(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr);
        //! Creates a frustum based on a (projection) matrix. The six planes of the frustum are derived from the matrix. To create a world space frustum, use a view-projection matrix.
        explicit Frustum(const glm::mat4& matrix);

        //! Creates a frustum based on the corners of a near and far portal.
        void set(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr);
        //! Creates a frustum based on a (projection) matrix. The six planes of the frustum are derived from the matrix. To create a world space frustum, use a view-projection matrix.
        void set(const glm::mat4& matrix);

        //! Returns true if point is within frustum.
        bool contains(const glm::vec3& loc) const;
        //! Returns true if the sphere is fully contained within frustum. See also 'intersects'.
        bool contains(const glm::vec3& center, float radius) const;
        //! Returns true if the box is fully contained within frustum. See also 'intersects'.
        bool contains(const glm::vec3& center, const glm::vec3& size) const;
        //! Returns true if the sphere is fully contained within frustum. See also 'intersects'.
        bool contains(const Sphere& sphere) const;
        //! Returns true if the box is fully contained within frustum. See also 'intersects'.
        bool contains(const AABB& box) const;
        //! Returns true if the rect is fully contained within frustum. See also 'intersects'.
        bool contains(const Rect& rect) const;

        //! Returns true if point is within frustum.
        bool intersects(const glm::vec3& loc) const;
        //! Returns true if the sphere is fully or partially contained within frustum. See also 'contains'.
        bool intersects(const glm::vec3& center, float radius) const;
        //! Returns true if the box is fully or partially contained within frustum. See also 'contains'.
        bool intersects(const glm::vec3& center, const glm::vec3& size) const;
        //! Returns true if the sphere is fully or partially contained within frustum. See also 'contains'.
        bool intersects(const Sphere& sphere) const;
        //! Returns true if the box is fully or partially contained within frustum. See also 'contains'.
        bool intersects(const AABB& box) const;
        //! Returns true if the rect is fully or partially contained within frustum. See also 'contains'.
        bool intersects(const Rect& rect) const;

        //! Transform the frustum by a given transform.
        void transform(const glm::mat4& transform);

        //! Returns a const reference to the Plane associated with /a index section of the Frustum.
        Plane& operator[](size_t index) { return planes[index]; }
        //! Returns a const reference to the Plane associated with /a index section of the Frustum.
        const Plane& operator[](size_t index) const { return planes[index]; }
        //! Returns a const reference to the Plane associated with /a section of the Frustum.
        Plane& operator[](FrustumSection section) { return planes[section]; }
        //! Returns a const reference to the Plane associated with /a section of the Frustum.
        const Plane& operator[](FrustumSection section) const { return planes[section]; }
        //!Returns a const reference to the Frustrum verticies.
        const std::array<glm::vec3, 8> getVertices() const { return vertices; }

    private:
        void calculateVertices(const glm::mat4& transform);

        std::array<Plane, 6> planes;
        std::array<glm::vec3, 8> vertices;
    };
}