#pragma once

#include "fusion/graphics/textures/texture.h"

namespace fe {
    /**
     * @brief Resource that represents a 2D image.
     */
    class FUSION_API Texture2d final : public Texture {
    public:
        Texture2d() = default;

        /**
         * Creates a new 2D image.
         * @param uuid The file uuid to load the image from.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         * @param load If this resource will be loaded immediately, otherwise {@link Texture2d#load} can be called later.
         */
        explicit Texture2d(uuids::uuid uuid,
                           VkFilter filter = VK_FILTER_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                           bool anisotropic = true,
                           bool mipmap = true,
                           bool load = false);

        /**
         * Creates a new 2D image.
         * @param extent The images extent in pixels.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2d(const glm::uvec2& extent,
                           VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                           VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                           VkFilter filter = VK_FILTER_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                           VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                           bool anisotropic = false,
                           bool mipmap = false);

        /**
         * Creates a new 2D image.
         * @param pixels The pixels to copy from.
         * @param extent The images extent in pixels.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2d(gsl::span<const uint8_t> pixels,
                           const glm::uvec2& extent,
                           VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                           VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                           VkFilter filter = VK_FILTER_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                           VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                           bool anisotropic = false,
                           bool mipmap = false);

        /**
         * Creates a new 2D image.
         * @param bitmap The bitmap to load from.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        explicit Texture2d(const std::unique_ptr<Bitmap>& bitmap,
                           VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                           VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                           VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                           VkFilter filter = VK_FILTER_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                           VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                           bool anisotropic = false,
                           bool mipmap = false);

        type_index getType() const override { return type_id<Texture2d>; }

        void load() override { loadFromFile(); };
        void unload() override { loaded = false; /*TODO: implement unload/reload feature*/ }

    private:
        void loadFromFile();
    };
}
