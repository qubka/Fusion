#pragma once

#include "image.hpp"

namespace fe {
    class Bitmap;

    /**
     * @brief Resource that represents a cubemap image.
     */
    class ImageCube : public Image {
    public:
        /**
         * Creates a new cubemap image.
         * @param filename The file to load the image from.
         * @param fileSuffix The files extension type (ex .png).
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         * @param load If this resource will be loaded immediately, otherwise {@link ImageCube#Load} can be called later.
         */
        explicit ImageCube(std::filesystem::path filename,
                           std::string fileSuffix = ".png",
                           VkFilter filter = VK_FILTER_LINEAR,
                           VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                           bool anisotropic = true,
                           bool mipmap = true,
                           bool load = true);

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
        explicit ImageCube(const glm::uvec2& extent,
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
        explicit ImageCube(std::unique_ptr<Bitmap>&& bitmap,
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

        /**
         * Sets the pixels of this image.
         * @param pixels The pixels to copy from.
         * @param layerCount The amount of layers contained in the pixels.
         * @param baseArrayLayer The first layer to copy into.
         */
        void setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer);

        const std::filesystem::path& getFilename() const { return filename; }
        const std::string& getFileSuffix() const { return fileSuffix; }
        const std::vector<std::string>& getFileSides() const { return fileSides; }
        bool isAnisotropic() const { return anisotropic; }
        bool isMipmap() const { return mipmap; }
        uint32_t getComponents() const { return components; }

    private:
        void load(std::unique_ptr<Bitmap> loadBitmap = nullptr);

        std::filesystem::path filename;
        std::string fileSuffix;
        /// X, -X, +Y, -Y, +Z, -Z
        std::vector<std::string> fileSides{ "Right", "Left", "Top", "Bottom", "Back", "Front" };

        uint32_t components{ 0 };
        bool anisotropic;
        bool mipmap;
    };
}
