#pragma once

/*namespace glm {
    GLM_INLINE glm::vec3 vec3_cast(const aiVector3D& v) { return {v.x, v.y, v.z}; }
    GLM_INLINE glm::vec3 vec3_cast(const aiColor3D& c) { return {c.r, c.g, c.b}; }
    GLM_INLINE glm::vec2 vec2_cast(const aiVector3D& v) { return {v.x, v.y}; }
    GLM_INLINE glm::quat quat_cast(const aiQuaternion& q) { return {q.w, q.x, q.y, q.z}; }
    GLM_INLINE glm::mat4 mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
    GLM_INLINE glm::mat4 mat4_cast(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }
    GLM_CONSTEXPR uint32_t hash(const char* s, int off = 0) { return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off]; }

    template<typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR T cross(const vec<2, T, Q>& x, const vec<2, T, Q>& y)  {
        return x.x * y.y - x.y * y.x;
    }
}*/

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

namespace glm {
    bool decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

    glm::vec3 smoothdamp(const glm::vec3& current, const glm::vec3& target, glm::vec3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
}
