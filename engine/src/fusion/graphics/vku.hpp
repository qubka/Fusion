#pragma once

#include <volk/volk.h>

namespace vku {
    /*template<typename T> static constexpr T numMipmapLevels(T width, T height) {
        T levels = 1;
        while((width|height) >> levels) {
            ++levels;
        }
        return levels;
    }

    /// Scale a value by mip level, but do not reduce to zero.
    inline uint32_t mipScale(uint32_t value, uint32_t mipLevel) {
        return std::max(value >> mipLevel, 1U);
    }*/

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
        std::memcpy(&result.float32, &v, sizeof(result.float32));
        return result;
    }

    // For converting between VULKAN and glm
    inline const VkOffset2D& ivec2_cast(const glm::ivec2& v) { return *reinterpret_cast<const VkOffset2D*>(&v); }
    inline const VkOffset3D& ivec3_cast(const glm::ivec3& v) { return *reinterpret_cast<const VkOffset3D*>(&v); }
    inline const VkExtent2D& uvec2_cast(const glm::uvec2& v) { return *reinterpret_cast<const VkExtent2D*>(&v); }
    inline const VkExtent3D& uvec3_cast(const glm::uvec3& v) { return *reinterpret_cast<const VkExtent3D*>(&v); }
    inline const glm::ivec2& offset2D_cast(const VkOffset2D& v) { return *reinterpret_cast<const glm::ivec2*>(&v); }
    inline const glm::ivec3& offset3D_cast(const VkOffset3D& v) { return *reinterpret_cast<const glm::ivec3*>(&v); }
    inline const glm::uvec2& extent2D_cast(const VkExtent2D& v) { return *reinterpret_cast<const glm::uvec2*>(&v); }
    inline const glm::uvec3& extent3D_cast(const VkExtent3D& v) { return *reinterpret_cast<const glm::uvec3*>(&v); }

    inline std::string StringifyResultVk(VkResult result) {
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

    inline void CheckVk(const std::string& filename, uint32_t line, VkResult result) {
        if (result != VK_SUCCESS) {
            auto failure = StringifyResultVk(result);
            LOG_ERROR << "Vulkan error (" << filename << ": " << line << ") - " << failure;
            throw std::runtime_error("Vulkan error: " + failure);
        }
    }

    /// Description of blocks for compressed formats.
    struct BlockParams {
        uint8_t width;
        uint8_t height;
        uint8_t bytes;
    };

    /// Get the details of vulkan texture formats.
    inline BlockParams getBlockParams(VkFormat format) {
        switch (format) {
            case VK_FORMAT_R4G4_UNORM_PACK8:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R5G6B5_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_B5G6R5_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8_UNORM:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_SNORM:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_USCALED:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_SSCALED:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_UINT:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_SINT:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8_SRGB:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_R8G8_UNORM:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_SNORM:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_USCALED:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_SSCALED:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_UINT:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_SINT:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8_SRGB:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R8G8B8_UNORM:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_SNORM:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_USCALED:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_SSCALED:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_UINT:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_SINT:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8_SRGB:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_UNORM:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_SNORM:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_USCALED:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_SSCALED:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_UINT:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_SINT:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_B8G8R8_SRGB:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_R8G8B8A8_UNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_SNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_USCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_SSCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_SINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R8G8B8A8_SRGB:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_UNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_SNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_USCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_SSCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_SINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_B8G8R8A8_SRGB:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_UINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_SINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_A2B10G10R10_SINT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16_UNORM:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_SNORM:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_USCALED:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_SSCALED:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_UINT:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_SINT:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16_SFLOAT:
                return BlockParams{ 1, 1, 2 };
            case VK_FORMAT_R16G16_UNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_SNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_USCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_SSCALED:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_SINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16_SFLOAT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R16G16B16_UNORM:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_SNORM:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_USCALED:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_SSCALED:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_UINT:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_SINT:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16_SFLOAT:
                return BlockParams{ 1, 1, 6 };
            case VK_FORMAT_R16G16B16A16_UNORM:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_SNORM:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_USCALED:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_SSCALED:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_UINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_SINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R32_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R32_SINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R32_SFLOAT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_R32G32_UINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R32G32_SINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R32G32_SFLOAT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R32G32B32_UINT:
                return BlockParams{ 1, 1, 12 };
            case VK_FORMAT_R32G32B32_SINT:
                return BlockParams{ 1, 1, 12 };
            case VK_FORMAT_R32G32B32_SFLOAT:
                return BlockParams{ 1, 1, 12 };
            case VK_FORMAT_R32G32B32A32_UINT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R32G32B32A32_SINT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R64_UINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R64_SINT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R64_SFLOAT:
                return BlockParams{ 1, 1, 8 };
            case VK_FORMAT_R64G64_UINT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R64G64_SINT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R64G64_SFLOAT:
                return BlockParams{ 1, 1, 16 };
            case VK_FORMAT_R64G64B64_UINT:
                return BlockParams{ 1, 1, 24 };
            case VK_FORMAT_R64G64B64_SINT:
                return BlockParams{ 1, 1, 24 };
            case VK_FORMAT_R64G64B64_SFLOAT:
                return BlockParams{ 1, 1, 24 };
            case VK_FORMAT_R64G64B64A64_UINT:
                return BlockParams{ 1, 1, 32 };
            case VK_FORMAT_R64G64B64A64_SINT:
                return BlockParams{ 1, 1, 32 };
            case VK_FORMAT_R64G64B64A64_SFLOAT:
                return BlockParams{ 1, 1, 32 };
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_D16_UNORM:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_D32_SFLOAT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_S8_UINT:
                return BlockParams{ 1, 1, 1 };
            case VK_FORMAT_D16_UNORM_S8_UINT:
                return BlockParams{ 1, 1, 3 };
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return BlockParams{ 1, 1, 4 };
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
                return BlockParams{ 4, 4, 8 };
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
                return BlockParams{ 4, 4, 8 };
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return BlockParams{ 4, 4, 8 };
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
                return BlockParams{ 4, 4, 8 };
            case VK_FORMAT_BC2_UNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC2_SRGB_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC3_SRGB_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC4_UNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC4_SNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
            case VK_FORMAT_BC5_SNORM_BLOCK:
                return BlockParams{ 4, 4, 16 };
        }
        throw std::runtime_error("Such format not exist in the table");
    }
}

#ifdef FUSION_DEBUG
#define VK_CHECK(func) { auto result = func; if (result != VK_SUCCESS) vku::CheckVk(__FILE__, __LINE__, result); }
#define VK_CHECK_RESULT(result) if (result != VK_SUCCESS) vku::CheckVk(__FILE__, __LINE__, result);
#else
#define VK_CHECK(func) func
#define VK_CHECK_RESULT(result)
#endif