#pragma once

#include "fusion/graphics/descriptors/descriptor.h"

namespace fe {
    class Bitmap;

    /**
     * @brief A representation of a Vulkan image, sampler, and view.
     */
    class Image : public Descriptor {
    public:
        Image() = default;

        /**
         * Creates a new image object.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param aspect The value specifying which aspects of an image are included in a view.
         * @param viewType The value specifying the type of the image view.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param extent The number of data elements in each dimension of the base level.
         */
        Image(VkFilter filter,
              VkSamplerAddressMode addressMode,
              VkSampleCountFlagBits samples,
              VkImageLayout layout,
              VkImageUsageFlags usage,
              VkImageAspectFlags aspect,
              VkImageViewType viewType,
              VkFormat format,
              const VkExtent3D& extent);
        ~Image() override;

        static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t count);

        /**
         * Copies the images pixels from memory to a bitmap. If this method is called from multiple threads at the same time Vulkan will crash!
         * @param mipLevel The mipmap level index to sample.
         * @param arrayLayer The array level to sample.
         * @return A copy of the images pixels.
         */
        std::unique_ptr<Bitmap> getBitmap(uint32_t mipLevel = 0, uint32_t arrayLayer = 0) const;

        const VkExtent3D& getExtent() const { return extent; }
        glm::uvec2 getSize() const { return {extent.width, extent.height}; }
        VkFormat getFormat() const { return format; }
        VkSampleCountFlagBits getSamples() const { return samples; }
        VkImageUsageFlags getUsage() const { return usage; }
        VkFilter getFilter() const { return filter; }
        VkSamplerAddressMode getAddressMode() const { return addressMode; }
        VkImageLayout getLayout() const { return layout; }
        VkImageAspectFlags getAspect() const { return aspect; }
        VkImageViewType getViewType() const { return viewType; }
        const VkImage& getImage() const { return image; }
        const VkDeviceMemory& getMemory() const { return memory; }
        const VkSampler& getSampler() const { return sampler; }
        const VkImageView& getView() const { return view; }

        operator const VkImage&() const { return image; }

        static uint32_t GetMipLevels(const VkExtent3D& extent);

        /**
         * Find a format in the candidates list that fits the tiling and features required.
         * @param candidates Formats that are tested for features, in order of preference.
         * @param tiling Tiling mode to test features in.
         * @param features The features to test for.
         * @return The format found, or VK_FORMAT_UNDEFINED.
         */
        static VkFormat FindSupportedFormat(std::span<const VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        /**
         * Gets if a format has a depth component.
         * @param format The format to check.
         * @return If the format has a depth component.
         */
        static bool HasDepth(VkFormat format);

        /**
         * Gets if a format has a depth component.
         * @param format The format to check.
         * @return If the format has a depth component.
         */
        static bool HasStencil(VkFormat format);

        static void CreateImage(
                VkImage& image,
                VkDeviceMemory& memory,
                const VkExtent3D& extent,
                VkFormat format,
                VkSampleCountFlagBits samples,
                VkImageTiling tiling,
                VkImageUsageFlags usage,
                VkMemoryPropertyFlags properties,
                uint32_t mipLevels,
                uint32_t arrayLayers,
                VkImageType imageType);

        static void CreateImageSampler(
                VkSampler& sampler,
                VkFilter filter,
                VkSamplerAddressMode addressMode,
                bool anisotropic,
                uint32_t mipLevels);

        static void CreateImageView(
                VkImage image,
                VkImageView& imageView,
                VkImageViewType viewType,
                VkFormat format,
                VkImageAspectFlags imageAspect,
                uint32_t mipLevels,
                uint32_t baseMipLevel,
                uint32_t layerCount,
                uint32_t baseArrayLayer);

        static void CreateMipmaps(
                VkImage image,
                const VkExtent3D& extent,
                VkFormat format,
                VkImageLayout dstImageLayout,
                uint32_t mipLevels,
                uint32_t baseArrayLayer,
                uint32_t layerCount);

        static void TransitionImageLayout(
                VkImage image,
                VkFormat format,
                VkImageLayout srcImageLayout,
                VkImageLayout dstImageLayout,
                VkImageAspectFlags imageAspect,
                uint32_t mipLevels,
                uint32_t baseMipLevel,
                uint32_t layerCount,
                uint32_t baseArrayLayer);

        static void InsertImageMemoryBarrier(
                VkCommandBuffer commandBuffer,
                VkImage image,
                VkAccessFlags srcAccessMask,
                VkAccessFlags dstAccessMask,
                VkImageLayout oldImageLayout,
                VkImageLayout newImageLayout,
                VkPipelineStageFlags srcStageMask,
                VkPipelineStageFlags dstStageMask,
                VkImageAspectFlags imageAspect,
                uint32_t mipLevels,
                uint32_t baseMipLevel,
                uint32_t layerCount,
                uint32_t baseArrayLayer);

        static void CopyBufferToImage(
                VkBuffer buffer,
                VkImage image,
                const VkExtent3D& extent,
                uint32_t layerCount,
                uint32_t baseArrayLayer);

        static bool CopyImage(
                VkImage srcImage,
                VkImage& dstImage,
                VkDeviceMemory& dstImageMemory,
                VkFormat srcFormat,
                VkFormat dstFormat,
                const VkExtent3D& extent,
                VkImageLayout srcImageLayout,
                uint32_t mipLevel,
                uint32_t arrayLayer);

        /*bool operator==(const Image& rhs) const {
            return extent == rhs.extent &&
                   samples == rhs.samples &&
                   usage == rhs.usage &&
                   format == rhs.format &&
                   filter == rhs.filter &&
                   addressMode == rhs.addressMode &&
                   viewType == rhs.viewType &&
                   aspect == rhs.aspect &&
                   layout == rhs.layout &&
                   image == rhs.image &&
                   memory == rhs.memory &&
                   sampler == rhs.sampler &&
                   view == rhs.view;
        }

        bool operator!=(const Image& rhs) const {
            return !operator==(rhs);
        }*/

    private:
        static VkPipelineStageFlags AccessFlagsToPipelineStage(VkAccessFlags accessFlags, VkPipelineStageFlags stageFlags);
        static VkPipelineStageFlags LayoutToAccessMask(VkImageLayout layout, bool isDestination);

    protected:
        VkExtent3D extent;
        VkSampleCountFlagBits samples;
        VkImageUsageFlags usage;
        VkFormat format{ VK_FORMAT_UNDEFINED };

        VkFilter filter;
        VkSamplerAddressMode addressMode;
        VkImageViewType viewType;
        VkImageAspectFlags aspect;
        VkImageLayout layout;

        VkImage image{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        VkSampler sampler{ VK_NULL_HANDLE };
        VkImageView view{ VK_NULL_HANDLE };
    };
}
