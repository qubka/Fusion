#pragma once

#include "fusion/graphics/textures/texture.h"

namespace fe {
    /**
     * @brief Resource that represents an array of 2D images.
     */
    class FUSION_API Texture2dArray final : public Texture {
    public:
        Texture2dArray() = default;

        /**
         * Creates a new array of 2D images.
         * @param uuid The file uuid to load the image from. (supports .ktx and .dds)
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         * @param load If this resource will be loaded immediately, otherwise {@link Texture2d#load} can be called later.
         */
        explicit Texture2dArray(uuids::uuid uuid,
                                VkFilter filter = VK_FILTER_LINEAR,
                                VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                bool anisotropic = true,
                                bool mipmap = true,
                                bool load = false);

        /**
         * Creates a new array of 2D images.
         * @param extent The images extent in pixels.
         * @param arrayLayers The number of layers in the image .
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2dArray(const glm::uvec2& extent,
                                uint32_t arrayLayers,
                                VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                                VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                                VkFilter filter = VK_FILTER_LINEAR,
                                VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                bool anisotropic = false,
                                bool mipmap = false);

        /**
         * Creates a new array of 2D images.
         * @param pixels The pixels to copy from.
         * @param extent The images extent in pixels.
         * @param arrayLayers The number of layers in the image .
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2dArray(gsl::span<const uint8_t> pixels,
                                const glm::uvec2& extent,
                                uint32_t arrayLayers,
                                VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                                VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                                VkFilter filter = VK_FILTER_LINEAR,
                                VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                bool anisotropic = false,
                                bool mipmap = false);

        /**
         * Creates a new array of 2D images.
         * @param bitmap The bitmap to load from.
         * @param arrayLayers The number of layers in the image .
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2dArray(const std::unique_ptr<Bitmap>& bitmap,
                                uint32_t arrayLayers,
                                VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                                VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                                VkFilter filter = VK_FILTER_LINEAR,
                                VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                bool anisotropic = false,
                                bool mipmap = false);

        type_index getType() const override { return type_id<Texture2dArray>; }

        void load() override { loadFromFile(); }
        void unload() override { loaded = false; /*TODO: implement unload/reload feature*/ }

    private:
        void loadFromFile();
    };
}
