#pragma once

#include <volk.h>

namespace vku {
    template<typename T> static constexpr T numMipmapLevels(T width, T height) {
        T levels = 1;
        while((width|height) >> levels) {
            ++levels;
        }
        return levels;
    }

    inline VkViewport viewport(float x, float y, float width, float height, float minDepth = 0, float maxDepth = 1) {
        return { x, y, width, height, minDepth, maxDepth };
    }

    inline VkViewport viewport(const glm::uvec2& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, 0, static_cast<float>(size.x), static_cast<float>(size.y), minDepth, maxDepth);
    }

    inline VkViewport viewport(const VkExtent2D& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, 0, static_cast<float>(size.width), static_cast<float>(size.height), minDepth, maxDepth);
    }

    inline VkViewport flippedViewport(const glm::uvec2& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, static_cast<float>(size.y), static_cast<float>(size.x), -static_cast<float>(size.y), minDepth, maxDepth);
    }

    inline VkViewport flippedViewport(const VkExtent2D& size, float minDepth = 0, float maxDepth = 1) {
        return viewport(0, static_cast<float>(size.height), static_cast<float>(size.width), -static_cast<float>(size.height), minDepth, maxDepth);
    }

    inline VkRect2D rect2D(uint32_t width, uint32_t height, int32_t offsetX = 0, int32_t offsetY = 0) {
        return {{offsetX,offsetY},{width,height}};
    }

    inline VkRect2D rect2D(const glm::uvec2& size, const glm::ivec2& offset = {}) {
        return rect2D(size.x, size.y, offset.x, offset.y);
    }

    inline VkRect2D rect2D(const VkExtent2D& size, const VkOffset2D& offset = {}) {
        return rect2D(size.width, size.height, offset.x, offset.y);
    }

    inline VkClearColorValue clearColor(const glm::vec4& v = {0.0f, 0.0f, 0.0f, 1.0f}) {
        VkClearColorValue result;
        memcpy(&result.float32, &v, sizeof(result.float32));
        return result;
    }

    // For converting between VULKAN and glm
    static inline const VkOffset2D& ivec2_cast(const glm::ivec2& v) { return *reinterpret_cast<const VkOffset2D*>(&v); }
    static inline const VkOffset3D& ivec3_cast(const glm::ivec3& v) { return *reinterpret_cast<const VkOffset3D*>(&v); }
    static inline const VkExtent2D& uvec2_cast(const glm::uvec2& v) { return *reinterpret_cast<const VkExtent2D*>(&v); }
    static inline const VkExtent3D& uvec3_cast(const glm::uvec3& v) { return *reinterpret_cast<const VkExtent3D*>(&v); }
    static inline const glm::ivec2& offset2D_cast(const VkOffset2D& v) { return *reinterpret_cast<const glm::ivec2*>(&v); }
    static inline const glm::ivec3& offset3D_cast(const VkOffset3D& v) { return *reinterpret_cast<const glm::ivec3*>(&v); }
    static inline const glm::uvec2& extent2D_cast(const VkExtent2D& v) { return *reinterpret_cast<const glm::uvec2*>(&v); }
    static inline const glm::uvec3& extent3D_cast(const VkExtent3D& v) { return *reinterpret_cast<const glm::uvec3*>(&v); }

    inline std::string to_string(VkResult result) {
        switch (result) {
            case VK_SUCCESS:
                return "Success";
            case VK_NOT_READY:
                return "A fence or query has not yet completed";
            case VK_TIMEOUT:
                return "A wait operation has not completed in the specified time";
            case VK_EVENT_SET:
                return "An event is signaled";
            case VK_EVENT_RESET:
                return "An event is unsignaled";
            case VK_INCOMPLETE:
                return "A return array was too small for the result";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "A host memory allocation has failed";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "A device memory allocation has failed";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "Initialization of an object could not be completed for implementation-specific reasons";
            case VK_ERROR_DEVICE_LOST:
                return "The logical or physical device has been lost";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "Mapping of a memory object has failed";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "A requested layer is not present or could not be loaded";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "A requested extension is not supported";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "A requested feature is not supported";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "Too many objects of the type have already been created";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "A requested format is not supported on this device";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "A surface is no longer available";
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                return "A allocation failed due to having no more space in the descriptor pool";
            case VK_SUBOPTIMAL_KHR:
                return "A swapchain no longer matches the surface properties exactly, but can still be used";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "A surface has changed in such a way that it is no longer compatible with the swapchain";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "The display used by a swapchain does not use the same presentable image layout";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
            case VK_ERROR_VALIDATION_FAILED_EXT:
                return "A validation layer found an error";
            default:
                return "Unknown Vulkan error";
        }
    }

    inline bool check_vk_errors(const std::string& filename, uint32_t line, VkResult result) {
        if (result != VK_SUCCESS) {
            LOG_ERROR << "Vulkan error (" << filename << ": " << line << ") - " << to_string(result);
            throw std::runtime_error("Vulkan error: " + vku::to_string(result));
            return false;
        }
        return true;
    }
}

#ifdef FUSION_DEBUG
#define VK_CHECK(func) { auto RESULT__ = func; if (RESULT__ != VK_SUCCESS) vku::check_vk_errors(__FILE__, __LINE__, RESULT__); }
#else
#define VK_RESULT(func) func
#endif

/*template<typename Error, typename Function, typename... Args>
auto vkCallImpl(const char* filename, const uint32_t line, Error&& error, Function&& function, Args&&...args)
-> typename std::enable_if_t<std::is_same_v<VkResult, decltype(function(args...))>, bool> {
    auto result = function(std::forward<Args>(args)...);
#ifdef FUSION_DEBUG
    return error(filename, line, result);
#else
    return true;
#endif
}

#define vkCall(function, ...) vkCallImpl(__FILE__, __LINE__, vku::check_vk_errors, function, __VA_ARGS__)*/