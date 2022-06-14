#pragma once

namespace glm {
    template<typename T, qualifier Q>
    GLM_FUNC_DECL bool decompose(mat<4, 4, T, Q> const& modelMatrix, vec<3, T, Q>& translation, vec<3, T, Q>& rotation, vec<3, T, Q>& scale) {
        mat<4, 4, T, Q> localMatrix{ modelMatrix };

        // Normalize the matrix.
        if (epsilonEqual(localMatrix[3][3], static_cast<T>(0), epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (
                epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
                epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
            localMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        translation = vec<3, T, Q>{ localMatrix[3] };
        localMatrix[3] = vec<4, T, Q>{ 0, 0, 0, localMatrix[3].w };

        vec<3, T, Q> Row[3], Pdum3;

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = localMatrix[i][j];

        // Compute X scale factor and normalize first row.
        scale.x = length(Row[0]);
        Row[0] = detail::scale(Row[0], static_cast<T>(1));
        scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
#if 0
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
            {
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2f(Row[1][2], Row[2][2]);
            rotation.z = atan2f(Row[0][1], Row[0][0]);
        } else {
            rotation.x = atan2f(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        return true;
    }

    template<typename T, qualifier Q>
    GLM_FUNC_DECL vec<3, T, Q> smoothdamp(vec<3, T, Q> const& current, vec<3, T, Q> const& target, vec<3, T, Q>& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        // Based on Game Programming Gems 4 Chapter 1.10
        smoothTime = max(0.0001f, smoothTime);
        float omega = 2.0f / smoothTime;

        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        vec<3, T, Q> change {current - target};

        // Clamp maximum speed
        float maxChange = maxSpeed * smoothTime;

        float maxChangeSq = maxChange * maxChange;
        T sqrMag = length2(change);
        if (sqrMag > maxChangeSq) {
            change /= sqrt(sqrMag) * maxChange;
        }

        vec<3, T, Q> dest { current - change };

        vec<3, T, Q> temp { (currentVelocity + omega * change) * deltaTime };

        currentVelocity = (currentVelocity - omega * temp) * exp;

        vec<3, T, Q> output = dest + (change + temp) * exp;

        // Prevent overshooting
        vec<3, T, Q> origMinusCurrent {target - current};
        vec<3, T, Q> outMinusOrig {output - target};

        if (dot(origMinusCurrent, outMinusOrig) > 0) {
            output = target;
            currentVelocity = {};
        }

        return output;
    }

    template<typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR T cross(const vec<2, T, Q>& x, const vec<2, T, Q>& y)  {
        return x.x * y.y - x.y * y.x;
    }
}

namespace vec3 {
    GLM_INLINE GLM_CONSTEXPR glm::vec3 right{1.0f, 0.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 left{-1.0f, 0.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 up{0.0f, 1.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 down{0.0f, -1.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 forward {0.0f, 0.0f, 1.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 back{0.0f ,0.0f, -1.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 zero{0.0f, 0.0f, 0.0f};
}
namespace vec2 {
    GLM_INLINE GLM_CONSTEXPR glm::vec2 right{1.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 left{-1.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 up{0.0f, 1.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 down{0.0f, -1.0f};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 zero{0.0f, 0.0f};
}
namespace quat {
    GLM_INLINE GLM_CONSTEXPR glm::quat identity{1.0f, 0.0f, 0.0f, 0.0f};
    GLM_INLINE GLM_CONSTEXPR glm::quat y_1800{0.0f, 0.0f, 1.0f, 0.0f};
}
