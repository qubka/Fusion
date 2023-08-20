#pragma once

#include "fusion/graphics/textures/texture.h"

namespace fe {
    /**
     * @brief Resource that represents a cubemap image.
     */
    class FUSION_API TextureCube final : public Texture {
    public:
        TextureCube() = default;

        /**
         * Creates a new cubemap image.
         * @param uuid The file uuid to load the image from. (supports .ktx and .dds)
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         * @param load If this resource will be loaded immediately, otherwise {@link ImageCube#load} can be called later.
         */
        explicit TextureCube(uuids::uuid uuid,
                             VkFilter filter = VK_FILTER_LINEAR,
                             VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                             bool anisotropic = true,
                             bool mipmap = true,
                             bool load = false);

        /**
         * Creates a new cubemap image.
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
        explicit TextureCube(const glm::uvec2& extent,
                             VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                             VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                             VkFilter filter = VK_FILTER_LINEAR,
                             VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                             VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                             bool anisotropic = false,
                             bool mipmap = false);

        /**
         * Creates a new cubemap image.
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
        explicit TextureCube(gsl::span<const uint8_t> pixels,
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
         * Creates a new cubemap image.
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
        explicit TextureCube(const std::unique_ptr<Bitmap>& bitmap,
                             VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                             VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
                             VkFilter filter = VK_FILTER_LINEAR,
                             VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                             VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                             bool anisotropic = false,
                             bool mipmap = false);

        /**
         * Copies the images pixels from memory to a bitmap. The bitmap height will be scaled by the amount of layers.
         * @param mipLevel The mipmap level index to sample.
         * @return A copy of the images pixels.
         */
        std::unique_ptr<Bitmap> getBitmap(uint32_t mipLevel = 0) const;

        type_index getType() const override { return type_id<TextureCube>; }

        void load() override { loadFromFile(); }
        void unload() override { loaded = false; /*TODO: implement unload/reload feature*/ }

    private:
        void loadFromFile();
    };
}
