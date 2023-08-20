#pragma once

#include <gli/format.hpp>
#include <gli/target.hpp>

namespace vku {
    template<typename T> static constexpr T numMipmapLevels(T width, T height) {
        T levels = 1;
        while((width|height) >> levels) {
            ++levels;
        }
        return levels;
    }

    /// Scale a value by mip level, but do not reduce to zero.
    inline uint32_t mipScale(uint32_t value, uint32_t mipLevel) {
        return glm::max(value >> mipLevel, 1U);
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
        return {{offsetX, offsetY},{width, height}};
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

    inline void CheckVk(std::string_view filename, uint32_t line, VkResult result) {
        if (result != VK_SUCCESS) {
            auto failure = StringifyResultVk(result);
            FE_LOG_ERROR("Vulkan error ('{}': {}) - {}", filename, line, failure);
            throw std::runtime_error("Vulkan error: " + failure);
        }
    }

    template<typename VkStruct>
    void AppendNextStruct(VkStruct& base, void* newStruct) {
        VkBaseOutStructure* next = (VkBaseOutStructure*) &base;

        while (next->pNext)
            next = next->pNext;

        next->pNext = (VkBaseOutStructure*) newStruct;
    }

    template<typename VkStruct>
    const VkBaseInStructure* FindNextStruct(const VkStruct* haystack, VkStructureType needle) {
        if (!haystack)
            return nullptr;

        const VkBaseInStructure* next = (const VkBaseInStructure*) haystack->pNext;
        while (next) {
            if (next->sType == needle)
                return next;

            next = next->pNext;
        }

        return nullptr;
    }

    template<typename VkStruct>
    VkBaseInStructure* FindNextStruct(VkStruct* haystack, VkStructureType needle) {
        if (!haystack)
            return nullptr;

        VkBaseInStructure* next = (VkBaseInStructure*) haystack->pNext;
        while (next) {
            if (next->sType == needle)
                return next;

            // assume non-const pNext in the original struct
            next = (VkBaseInStructure*) next->pNext;
        }

        return nullptr;
    }

    template<typename VkStruct>
    bool RemoveNextStruct(VkStruct* haystack, VkStructureType needle) {
        bool ret = false;

        // start from the haystack, and iterate
        VkBaseInStructure* root = (VkBaseInStructure*) haystack;
        while (root && root->pNext) {
            // at each point, if the *next* struct is the needle, then point our next pointer at whatever
            // its was - either the next in the chain or NULL if it was at the end. Then we can return true
            // because we removed the struct. We keep going to handle duplicates, but we continue and skip
            // the list iterate as we now have a new root->pNext.
            // Note that this can't remove the first struct in the chain but that's expected, we only want
            // to remove extension structs.
            if (root->pNext->sType == needle) {
                root->pNext = root->pNext->pNext;
                ret = true;
                continue;
            }

            // move to the next struct
            root = (VkBaseInStructure*) root->pNext;
        }

        return ret;
    }

    inline void rgba_to_bgra(uint8_t* dst, const uint8_t* src, size_t count) {
        for (size_t i = 0; i < count; ++i, src += 4) {
            *dst++ = src[2]; // b
            *dst++ = src[1]; // g
            *dst++ = src[0]; // r
            *dst++ = src[3]; // a
        }
    }

    // https://stackoverflow.com/questions/7069090/convert-rgb-to-rgba-in-c
    inline void rgb_to_rgba(uint8_t* dst, const uint8_t* src, size_t count) {
        if (count == 0)
            return;
        for (size_t i = count; --i; dst += 4, src += 3) {
            *(uint32_t*)(void*)dst = *(const uint32_t*)(const void*)src;
        }
        for (int j = 0; j < 3; ++j) {
            dst[j] = src[j];
        }
    }

    /*** GLI UTILS ***/

    /// Description of blocks for compressed formats.
    struct BlockParams {
        uint8_t width;
        uint8_t height;
        uint8_t bytes;
    };

    /// Get the details of vulkan texture formats.
    inline BlockParams get_format_params(VkFormat format) {
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

    inline VkImageType convert_type(gli::target target) {
        switch (target) {
            case gli::TARGET_1D:
            case gli::TARGET_1D_ARRAY:
                return VK_IMAGE_TYPE_1D;
            case gli::TARGET_2D:
            case gli::TARGET_2D_ARRAY:
            case gli::TARGET_RECT:
            case gli::TARGET_RECT_ARRAY:
            case gli::TARGET_CUBE:
            case gli::TARGET_CUBE_ARRAY:
                return VK_IMAGE_TYPE_2D;
            case gli::TARGET_3D:
                return VK_IMAGE_TYPE_3D;
            default:
                throw std::runtime_error("Unknown target");
        }
    }

    /*VkExtent3D convert_extent(const gli::extent3d& extent) {
        return { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), static_cast<uint32_t>(extent.z) };
    }

    VkExtent3D convert_extent(const gli::extent2d& extent) {
        return { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), 1 };
    }*/

    inline VkFormat convert_format(gli::format format) {
        switch (format) {
            case gli::FORMAT_RG4_UNORM_PACK8:
                return VK_FORMAT_R4G4_UNORM_PACK8;
            case gli::FORMAT_RGBA4_UNORM_PACK16:
                return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            case gli::FORMAT_BGRA4_UNORM_PACK16:
                return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            case gli::FORMAT_R5G6B5_UNORM_PACK16:
                return VK_FORMAT_R5G6B5_UNORM_PACK16;
            case gli::FORMAT_B5G6R5_UNORM_PACK16:
                return VK_FORMAT_B5G6R5_UNORM_PACK16;
            case gli::FORMAT_RGB5A1_UNORM_PACK16:
                return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            case gli::FORMAT_BGR5A1_UNORM_PACK16:
                return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            case gli::FORMAT_A1RGB5_UNORM_PACK16:
                return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            case gli::FORMAT_R8_UNORM_PACK8:
                return VK_FORMAT_R8_UNORM;
            case gli::FORMAT_R8_SNORM_PACK8:
                return VK_FORMAT_R8_SNORM;
            case gli::FORMAT_R8_USCALED_PACK8:
                return VK_FORMAT_R8_USCALED;
            case gli::FORMAT_R8_SSCALED_PACK8:
                return VK_FORMAT_R8_SSCALED;
            case gli::FORMAT_R8_UINT_PACK8:
                return VK_FORMAT_R8_UINT;
            case gli::FORMAT_R8_SINT_PACK8:
                return VK_FORMAT_R8_SINT;
            case gli::FORMAT_R8_SRGB_PACK8:
                return VK_FORMAT_R8_SRGB;
            case gli::FORMAT_RG8_UNORM_PACK8:
                return VK_FORMAT_R8G8_UNORM;
            case gli::FORMAT_RG8_SNORM_PACK8:
                return VK_FORMAT_R8G8_SNORM;
            case gli::FORMAT_RG8_USCALED_PACK8:
                return VK_FORMAT_R8G8_USCALED;
            case gli::FORMAT_RG8_SSCALED_PACK8:
                return VK_FORMAT_R8G8_SSCALED;
            case gli::FORMAT_RG8_UINT_PACK8:
                return VK_FORMAT_R8G8_UINT;
            case gli::FORMAT_RG8_SINT_PACK8:
                return VK_FORMAT_R8G8_SINT;
            case gli::FORMAT_RG8_SRGB_PACK8:
                return VK_FORMAT_R8G8_SRGB;
            case gli::FORMAT_RGB8_UNORM_PACK8:
                return VK_FORMAT_R8G8B8_UNORM;
            case gli::FORMAT_RGB8_SNORM_PACK8:
                return VK_FORMAT_R8G8B8_SNORM;
            case gli::FORMAT_RGB8_USCALED_PACK8:
                return VK_FORMAT_R8G8B8_USCALED;
            case gli::FORMAT_RGB8_SSCALED_PACK8:
                return VK_FORMAT_R8G8B8_SSCALED;
            case gli::FORMAT_RGB8_UINT_PACK8:
                return VK_FORMAT_R8G8B8_UINT;
            case gli::FORMAT_RGB8_SINT_PACK8:
                return VK_FORMAT_R8G8B8_SINT;
            case gli::FORMAT_RGB8_SRGB_PACK8:
                return VK_FORMAT_R8G8B8_SRGB;
            case gli::FORMAT_BGR8_UNORM_PACK8:
                return VK_FORMAT_B8G8R8_UNORM;
            case gli::FORMAT_BGR8_SNORM_PACK8:
                return VK_FORMAT_B8G8R8_SNORM;
            case gli::FORMAT_BGR8_USCALED_PACK8:
                return VK_FORMAT_B8G8R8_USCALED;
            case gli::FORMAT_BGR8_SSCALED_PACK8:
                return VK_FORMAT_B8G8R8_SSCALED;
            case gli::FORMAT_BGR8_UINT_PACK8:
                return VK_FORMAT_B8G8R8_UINT;
            case gli::FORMAT_BGR8_SINT_PACK8:
                return VK_FORMAT_B8G8R8_SINT;
            case gli::FORMAT_BGR8_SRGB_PACK8:
                return VK_FORMAT_B8G8R8_SRGB;
            case gli::FORMAT_RGBA8_UNORM_PACK8:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case gli::FORMAT_RGBA8_SNORM_PACK8:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case gli::FORMAT_RGBA8_USCALED_PACK8:
                return VK_FORMAT_R8G8B8A8_USCALED;
            case gli::FORMAT_RGBA8_SSCALED_PACK8:
                return VK_FORMAT_R8G8B8A8_SSCALED;
            case gli::FORMAT_RGBA8_UINT_PACK8:
                return VK_FORMAT_R8G8B8A8_UINT;
            case gli::FORMAT_RGBA8_SINT_PACK8:
                return VK_FORMAT_R8G8B8A8_SINT;
            case gli::FORMAT_RGBA8_SRGB_PACK8:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case gli::FORMAT_BGRA8_UNORM_PACK8:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case gli::FORMAT_BGRA8_SNORM_PACK8:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case gli::FORMAT_BGRA8_USCALED_PACK8:
                return VK_FORMAT_B8G8R8A8_USCALED;
            case gli::FORMAT_BGRA8_SSCALED_PACK8:
                return VK_FORMAT_B8G8R8A8_SSCALED;
            case gli::FORMAT_BGRA8_UINT_PACK8:
                return VK_FORMAT_B8G8R8A8_UINT;
            case gli::FORMAT_BGRA8_SINT_PACK8:
                return VK_FORMAT_B8G8R8A8_SINT;
            case gli::FORMAT_BGRA8_SRGB_PACK8:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case gli::FORMAT_RGBA8_UNORM_PACK32:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case gli::FORMAT_RGBA8_SNORM_PACK32:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case gli::FORMAT_RGBA8_USCALED_PACK32:
                return VK_FORMAT_R8G8B8A8_USCALED;
            case gli::FORMAT_RGBA8_SSCALED_PACK32:
                return VK_FORMAT_R8G8B8A8_SSCALED;
            case gli::FORMAT_RGBA8_UINT_PACK32:
                return VK_FORMAT_R8G8B8A8_UINT;
            case gli::FORMAT_RGBA8_SINT_PACK32:
                return VK_FORMAT_R8G8B8A8_SINT;
            case gli::FORMAT_RGBA8_SRGB_PACK32:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case gli::FORMAT_RGB10A2_UNORM_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_UNORM_PACK32");
            case gli::FORMAT_RGB10A2_SNORM_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_SNORM_PACK32");
            case gli::FORMAT_RGB10A2_USCALED_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_USCALED_PACK32");
            case gli::FORMAT_RGB10A2_SSCALED_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_SSCALED_PACK32");
            case gli::FORMAT_RGB10A2_UINT_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_UINT_PACK32");
            case gli::FORMAT_RGB10A2_SINT_PACK32:
                throw std::runtime_error("Unsupported FORMAT_RGB10A2_SINT_PACK32");
            case gli::FORMAT_BGR10A2_UNORM_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_UNORM_PACK32");
            case gli::FORMAT_BGR10A2_SNORM_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_SNORM_PACK32");
            case gli::FORMAT_BGR10A2_USCALED_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_USCALED_PACK32");
            case gli::FORMAT_BGR10A2_SSCALED_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_SSCALED_PACK32");
            case gli::FORMAT_BGR10A2_UINT_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_UINT_PACK32");
            case gli::FORMAT_BGR10A2_SINT_PACK32:
                throw std::runtime_error("Unsupported FORMAT_BGR10A2_SINT_PACK32");
            case gli::FORMAT_R16_UNORM_PACK16:
                return VK_FORMAT_R16_UNORM;
            case gli::FORMAT_R16_SNORM_PACK16:
                return VK_FORMAT_R16_SNORM;
            case gli::FORMAT_R16_USCALED_PACK16:
                return VK_FORMAT_R16_USCALED;
            case gli::FORMAT_R16_SSCALED_PACK16:
                return VK_FORMAT_R16_SSCALED;
            case gli::FORMAT_R16_UINT_PACK16:
                return VK_FORMAT_R16_UINT;
            case gli::FORMAT_R16_SINT_PACK16:
                return VK_FORMAT_R16_SINT;
            case gli::FORMAT_R16_SFLOAT_PACK16:
                return VK_FORMAT_R16_SFLOAT;
            case gli::FORMAT_RG16_UNORM_PACK16:
                return VK_FORMAT_R16_UNORM;
            case gli::FORMAT_RG16_SNORM_PACK16:
                return VK_FORMAT_R16_SNORM;
            case gli::FORMAT_RG16_USCALED_PACK16:
                return VK_FORMAT_R16G16_USCALED;
            case gli::FORMAT_RG16_SSCALED_PACK16:
                return VK_FORMAT_R16G16_SSCALED;
            case gli::FORMAT_RG16_UINT_PACK16:
                return VK_FORMAT_R16G16_UINT;
            case gli::FORMAT_RG16_SINT_PACK16:
                return VK_FORMAT_R16G16_SINT;
            case gli::FORMAT_RG16_SFLOAT_PACK16:
                return VK_FORMAT_R16G16_SFLOAT;
            case gli::FORMAT_RGB16_UNORM_PACK16:
                return VK_FORMAT_R16G16B16_UNORM;
            case gli::FORMAT_RGB16_SNORM_PACK16:
                return VK_FORMAT_R16G16B16_SNORM;
            case gli::FORMAT_RGB16_USCALED_PACK16:
                return VK_FORMAT_R16G16B16_USCALED;
            case gli::FORMAT_RGB16_SSCALED_PACK16:
                return VK_FORMAT_R16G16B16_SSCALED;
            case gli::FORMAT_RGB16_UINT_PACK16:
                return VK_FORMAT_R16G16B16_UINT;
            case gli::FORMAT_RGB16_SINT_PACK16:
                return VK_FORMAT_R16G16B16_SINT;
            case gli::FORMAT_RGB16_SFLOAT_PACK16:
                return VK_FORMAT_R16G16B16_SFLOAT;
            case gli::FORMAT_RGBA16_UNORM_PACK16:
                return VK_FORMAT_R16G16B16A16_UNORM;
            case gli::FORMAT_RGBA16_SNORM_PACK16:
                return VK_FORMAT_R16G16B16_SNORM;
            case gli::FORMAT_RGBA16_USCALED_PACK16:
                return VK_FORMAT_R16G16B16A16_USCALED;
            case gli::FORMAT_RGBA16_SSCALED_PACK16:
                return VK_FORMAT_R16G16B16A16_SSCALED;
            case gli::FORMAT_RGBA16_UINT_PACK16:
                return VK_FORMAT_R16G16B16A16_UINT;
            case gli::FORMAT_RGBA16_SINT_PACK16:
                return VK_FORMAT_R16G16B16A16_SINT;
            case gli::FORMAT_RGBA16_SFLOAT_PACK16:
                return VK_FORMAT_R16G16B16A16_SFLOAT;
            case gli::FORMAT_R32_UINT_PACK32:
                return VK_FORMAT_R32_UINT;
            case gli::FORMAT_R32_SINT_PACK32:
                return VK_FORMAT_R32_SINT;
            case gli::FORMAT_R32_SFLOAT_PACK32:
                return VK_FORMAT_R32_SFLOAT;
            case gli::FORMAT_RG32_UINT_PACK32:
                return VK_FORMAT_R32G32_UINT;
            case gli::FORMAT_RG32_SINT_PACK32:
                return VK_FORMAT_R32G32_SINT;
            case gli::FORMAT_RG32_SFLOAT_PACK32:
                return VK_FORMAT_R32G32_SFLOAT;
            case gli::FORMAT_RGB32_UINT_PACK32:
                return VK_FORMAT_R32G32B32_UINT;
            case gli::FORMAT_RGB32_SINT_PACK32:
                return VK_FORMAT_R32G32B32_SINT;
            case gli::FORMAT_RGB32_SFLOAT_PACK32:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case gli::FORMAT_RGBA32_UINT_PACK32:
                return VK_FORMAT_R32G32B32A32_UINT;
            case gli::FORMAT_RGBA32_SINT_PACK32:
                return VK_FORMAT_R32G32B32A32_SINT;
            case gli::FORMAT_RGBA32_SFLOAT_PACK32:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case gli::FORMAT_R64_UINT_PACK64:
                return VK_FORMAT_R64_UINT;
            case gli::FORMAT_R64_SINT_PACK64:
                return VK_FORMAT_R64_SINT;
            case gli::FORMAT_R64_SFLOAT_PACK64:
                return VK_FORMAT_R64_SFLOAT;
            case gli::FORMAT_RG64_UINT_PACK64:
                return VK_FORMAT_R64G64_UINT;
            case gli::FORMAT_RG64_SINT_PACK64:
                return VK_FORMAT_R64G64_SINT;
            case gli::FORMAT_RG64_SFLOAT_PACK64:
                return VK_FORMAT_R64G64_SFLOAT;
            case gli::FORMAT_RGB64_UINT_PACK64:
                return VK_FORMAT_R64G64B64_UINT;
            case gli::FORMAT_RGB64_SINT_PACK64:
                return VK_FORMAT_R64G64B64_SINT;
            case gli::FORMAT_RGB64_SFLOAT_PACK64:
                return VK_FORMAT_R64G64B64_SFLOAT;
            case gli::FORMAT_RGBA64_UINT_PACK64:
                return VK_FORMAT_R64G64B64A64_UINT;
            case gli::FORMAT_RGBA64_SINT_PACK64:
                return VK_FORMAT_R64G64B64A64_SINT;
            case gli::FORMAT_RGBA64_SFLOAT_PACK64:
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            case gli::FORMAT_RG11B10_UFLOAT_PACK32:
                throw std::runtime_error("Unsupported format FORMAT_RG11B10_UFLOAT_PACK32");
            case gli::FORMAT_RGB9E5_UFLOAT_PACK32:
                throw std::runtime_error("Unsupported format FORMAT_RGB9E5_UFLOAT_PACK32");
            case gli::FORMAT_D16_UNORM_PACK16:
                return VK_FORMAT_D16_UNORM;
            case gli::FORMAT_D24_UNORM_PACK32:
                throw std::runtime_error("Unsupported format FORMAT_D24_UNORM_PACK32");
            case gli::FORMAT_D32_SFLOAT_PACK32:
                return VK_FORMAT_D32_SFLOAT;
            case gli::FORMAT_S8_UINT_PACK8:
                return VK_FORMAT_S8_UINT;
            case gli::FORMAT_D16_UNORM_S8_UINT_PACK32:
                return VK_FORMAT_D16_UNORM_S8_UINT;
            case gli::FORMAT_D24_UNORM_S8_UINT_PACK32:
                return VK_FORMAT_D24_UNORM_S8_UINT;
            case gli::FORMAT_D32_SFLOAT_S8_UINT_PACK64:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
            case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8:
                return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            case gli::FORMAT_RGB_DXT1_SRGB_BLOCK8:
                return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
            case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
                return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            case gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8:
                return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
            case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
                return VK_FORMAT_BC2_UNORM_BLOCK;
            case gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16:
                return VK_FORMAT_BC2_SRGB_BLOCK;
            case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
                return VK_FORMAT_BC3_UNORM_BLOCK;
            case gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16:
                return VK_FORMAT_BC3_SRGB_BLOCK;
            case gli::FORMAT_R_ATI1N_UNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_R_ATI1N_UNORM_BLOCK8");
            case gli::FORMAT_R_ATI1N_SNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_R_ATI1N_UNORM_BLOCK8");
            case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RG_ATI2N_UNORM_BLOCK16");
            case gli::FORMAT_RG_ATI2N_SNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RG_ATI2N_SNORM_BLOCK16");
            case gli::FORMAT_RGB_BP_UFLOAT_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGB_BP_UFLOAT_BLOCK16");
            case gli::FORMAT_RGB_BP_SFLOAT_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGB_BP_SFLOAT_BLOCK16");
            case gli::FORMAT_RGBA_BP_UNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_BP_UNORM_BLOCK16");
            case gli::FORMAT_RGBA_BP_SRGB_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_BP_SRGB_BLOCK16");
            case gli::FORMAT_RGB_ETC2_UNORM_BLOCK8:
                return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
                throw std::runtime_error("Unsupported format FORMAT_RGB_ETC2_UNORM_BLOCK8");
            case gli::FORMAT_RGB_ETC2_SRGB_BLOCK8:
                return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8:
                return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ETC2_SRGB_BLOCK8:
                return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_ETC2_UNORM_BLOCK16");
            case gli::FORMAT_RGBA_ETC2_SRGB_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_ETC2_SRGB_BLOCK16");
            case gli::FORMAT_R_EAC_UNORM_BLOCK8:
                return VK_FORMAT_EAC_R11_UNORM_BLOCK;
            case gli::FORMAT_R_EAC_SNORM_BLOCK8:
                return VK_FORMAT_EAC_R11_SNORM_BLOCK;
            case gli::FORMAT_RG_EAC_UNORM_BLOCK16:
                return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
            case gli::FORMAT_RG_EAC_SNORM_BLOCK16:
                return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_4X4_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_5X4_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_5X5_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_6X5_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_6X6_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X5_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X6_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_8X8_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X5_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X6_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X8_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_10X10_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_12X10_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
            case gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16:
                return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
            case gli::FORMAT_RGBA_ASTC_12X12_SRGB_BLOCK16:
                return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
            case gli::FORMAT_RGB_PVRTC1_8X8_UNORM_BLOCK32:
                return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
            case gli::FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32");
            case gli::FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32");
            case gli::FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32");
            case gli::FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32");
            case gli::FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32");
            case gli::FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32");
            case gli::FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32");
            case gli::FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8");
            case gli::FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8");
            case gli::FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8");
            case gli::FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8");
            case gli::FORMAT_RGB_ETC_UNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGB_ETC_UNORM_BLOCK8");
            case gli::FORMAT_RGB_ATC_UNORM_BLOCK8:
                throw std::runtime_error("Unsupported format FORMAT_RGB_ATC_UNORM_BLOCK8");
            case gli::FORMAT_RGBA_ATCA_UNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_ATCA_UNORM_BLOCK16");
            case gli::FORMAT_RGBA_ATCI_UNORM_BLOCK16:
                throw std::runtime_error("Unsupported format FORMAT_RGBA_ATCI_UNORM_BLOCK16");
            case gli::FORMAT_L8_UNORM_PACK8:
                return VK_FORMAT_R8_UNORM;
            case gli::FORMAT_A8_UNORM_PACK8:
                return VK_FORMAT_R8_UNORM;
            case gli::FORMAT_LA8_UNORM_PACK8:
                return VK_FORMAT_R8G8_UNORM;
            case gli::FORMAT_L16_UNORM_PACK16:
                return VK_FORMAT_R16_UNORM;
            case gli::FORMAT_A16_UNORM_PACK16:
                return VK_FORMAT_R16_UNORM;
            case gli::FORMAT_LA16_UNORM_PACK16:
                return VK_FORMAT_R16G16_UNORM;
            case gli::FORMAT_BGR8_UNORM_PACK32:
                return VK_FORMAT_B8G8R8_UNORM;
            case gli::FORMAT_BGR8_SRGB_PACK32:
                return VK_FORMAT_B8G8R8_SRGB;
            case gli::FORMAT_RG3B2_UNORM_PACK8:
                throw std::runtime_error("Unsupported format FORMAT_RG3B2_UNORM_PACK8");
            default:
                throw std::runtime_error("Unknown format");
        }
    }
}

#ifdef FUSION_DEBUG
#define VK_CHECK(func) { auto result = func; if (result != VK_SUCCESS) vku::CheckVk(__FILE__, __LINE__, result); }
#define VK_CHECK_RESULT(result) if ((result) != VK_SUCCESS) vku::CheckVk(__FILE__, __LINE__, result);
#else
#define VK_CHECK(func) func
#define VK_CHECK_RESULT(result)
#endif

static inline bool operator==(const VkDescriptorSetLayoutBinding& lhs, const VkDescriptorSetLayoutBinding& rhs) {
    return lhs.binding == rhs.binding && lhs.descriptorType == rhs.descriptorType && lhs.descriptorCount == rhs.descriptorCount && lhs.stageFlags == rhs.stageFlags;
    // TODO: pImmutableSamplers ?
}

static inline bool operator!=(const VkDescriptorSetLayoutBinding& lhs, const VkDescriptorSetLayoutBinding& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkPushConstantRange& lhs, const VkPushConstantRange& rhs) {
    return lhs.stageFlags == rhs.stageFlags && lhs.offset == rhs.offset && lhs.size == rhs.size;
}

static inline bool operator!=(const VkPushConstantRange& lhs, const VkPushConstantRange& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkSamplerCreateInfo& lhs, const VkSamplerCreateInfo& rhs) {
    return lhs.sType == rhs.sType &&
           lhs.pNext == rhs.pNext &&
           lhs.flags == rhs.flags &&
           lhs.magFilter == rhs.magFilter &&
           lhs.minFilter == rhs.minFilter &&
           lhs.mipmapMode == rhs.mipmapMode &&
           lhs.addressModeU == rhs.addressModeU &&
           lhs.addressModeV == rhs.addressModeV &&
           lhs.addressModeW == rhs.addressModeW &&
           lhs.mipLodBias == rhs.mipLodBias &&
           lhs.anisotropyEnable == rhs.anisotropyEnable &&
           lhs.maxAnisotropy == rhs.maxAnisotropy &&
           lhs.compareEnable == rhs.compareEnable &&
           lhs.compareOp == rhs.compareOp &&
           lhs.minLod == rhs.minLod &&
           lhs.maxLod == rhs.maxLod &&
           lhs.borderColor == rhs.borderColor &&
           lhs.unnormalizedCoordinates == rhs.unnormalizedCoordinates;
}

static inline bool operator!=(const VkSamplerCreateInfo& lhs, const VkSamplerCreateInfo& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkExtent3D& lhs, const VkExtent3D& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height && lhs.depth == rhs.depth;
}

static inline bool operator!=(const VkExtent3D& lhs, const VkExtent3D& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkExtent2D& lhs, const VkExtent2D& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

static inline bool operator!=(const VkExtent2D& lhs, const VkExtent2D& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkOffset3D& lhs, const VkOffset3D& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

static inline bool operator!=(const VkOffset3D& lhs, const VkOffset3D& rhs) {
    return !(lhs == rhs);
}

static inline bool operator==(const VkOffset2D& lhs, const VkOffset2D& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

static inline bool operator!=(const VkOffset2D& lhs, const VkOffset2D& rhs) {
    return !(lhs == rhs);
}
