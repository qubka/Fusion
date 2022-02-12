#ifndef VOX_EXTENTIONS_H
#define VOX_EXTENTIONS_H

/*#include <assimp/types.h>

namespace glm {
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

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
};

namespace vec3 {
    GLM_INLINE GLM_CONSTEXPR glm::vec3 right = glm::vec3{1,0,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 left = glm::vec3{-1,0,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 up = glm::vec3{0,1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 down = glm::vec3{0,-1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 forward = glm::vec3{0,0,1};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 back = glm::vec3{0,0,-1};
    GLM_INLINE GLM_CONSTEXPR glm::vec3 zero = glm::vec3{0,0,0};
}
namespace vec2 {
    GLM_INLINE GLM_CONSTEXPR glm::vec2 right = glm::vec2{1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 left = glm::vec2{-1,0};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 up = glm::vec2{0,1};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 down = glm::vec2{0,-1};
    GLM_INLINE GLM_CONSTEXPR glm::vec2 zero = glm::vec2{0,0};
}

/*#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#include <glad/glad.h>

GLint glxGpuTotalMemory();
GLint glxGpuAvailMemory();*/

#endif //VOX_EXTENTIONS_H