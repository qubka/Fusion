#pragma once

#include "Plane.hpp"

namespace Fusion {
    class Sphere;
    class AABB;

    /// @brief Create frustum from matrix
    /// if extracted from projection matrix only, planes will be in eye-space
    /// if extracted from view*projection, planes will be in world space
    /// if extracted from model*view*projection planes will be in model space
    class FUSION_API Frustum {
        Plane planes[6];
    public:
        enum FrustumSection { TOP, BOTTOM, LEFT, RIGHT, NEAR, FAR };

        //! Creates a frustum based on the corners of a near and far portal.
        Frustum(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr);
        //! Creates a frustum based on a (projection) matrix. The six planes of the frustum are derived from the matrix. To create a world space frustum, use a view-projection matrix.
        explicit Frustum(const glm::mat4& mat);

        //! Creates a frustum based on the corners of a near and far portal.
        void set(const glm::vec3& ntl, const glm::vec3& ntr, const glm::vec3& nbl, const glm::vec3& nbr, const glm::vec3& ftl, const glm::vec3& ftr, const glm::vec3& fbl, const glm::vec3& fbr);
        //! Creates a frustum based on a (projection) matrix. The six planes of the frustum are derived from the matrix. To create a world space frustum, use a view-projection matrix.
        void set(const glm::mat4& mat);

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

        //! Returns a const reference to the Plane associated with /a section of the Frustum.
        Plane& operator[](FrustumSection section) { return planes[section]; }
        //! Returns a const reference to the Plane associated with /a section of the Frustum.
        const Plane& operator[](FrustumSection section) const { return planes[section]; }
    };
}