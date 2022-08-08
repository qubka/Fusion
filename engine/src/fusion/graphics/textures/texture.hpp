#pragma once

#include "fusion/graphics/textures/image.hpp"
#include "fusion/assets/asset.hpp"

namespace fe {
    /**
     * @brief Defines the purpose of a texture
     */
    /*enum class TextureType : unsigned char {
        None = 0,
        Diffuse = 1,
        Specular = 2,
        Ambient = 3,
        Emissive = 4,
        Height = 5,
        Normals = 6,
        Shininess = 7,
        Opacity = 8,
        Displacement = 9,
        Lightmap = 10,
        Reflection = 11,
        BaseColor = 12,
        NormalCamera = 13,
        EmissionColor = 14,
        Metalness = 15,
        DiffuseRoughness = 16,
        AmbientOcclusion = 17,
        Unknown = 18,
        Sheen = 19,
        Clearcoat = 20,
        Transmission = 21,
    };*/

    class Texture : public Image, public Asset {
    public:
        /**
         * Creates a new image.
         * @param path
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param aspect The value specifying which aspects of an image are included in a view.
         * @param viewType The value specifying the type of the image view.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param mipLevels The number of levels of detail available for minified sampling of the image.
         * @param arrayLayers The number of layers in the image.
         * @param extent The number of data elements in each dimension of the base level.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        Texture(const fs::path& filepath,
                VkFilter filter,
                VkSamplerAddressMode addressMode,
                VkSampleCountFlagBits samples,
                VkImageLayout layout,
                VkImageUsageFlags usage,
                VkImageAspectFlags aspect,
                VkImageViewType viewType,
                VkFormat format,
                uint32_t mipLevels,
                uint32_t arrayLayers,
                const VkExtent3D& extent,
                bool anisotropic,
                bool mipmap);

        /**
         * Creates a new image.
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param aspect The value specifying which aspects of an image are included in a view.
         * @param viewType The value specifying the type of the image view.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param mipLevels The number of levels of detail available for minified sampling of the image.
         * @param arrayLayers The number of layers in the image.
         * @param extent The number of data elements in each dimension of the base level.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        Texture(VkFilter filter,
                VkSamplerAddressMode addressMode,
                VkSampleCountFlagBits samples,
                VkImageLayout layout,
                VkImageUsageFlags usage,
                VkImageAspectFlags aspect,
                VkImageViewType viewType,
                VkFormat format,
                uint32_t mipLevels,
                uint32_t arrayLayers,
                const VkExtent3D& extent,
                bool anisotropic,
                bool mipmap);

        /**
         * Creates a new image.
         * @param bitmap
         * @param filter The magnification/minification filter to apply to lookups.
         * @param addressMode The addressing mode for outside [0..1] range.
         * @param samples The number of samples per texel.
         * @param layout The layout that the image subresources accessible from.
         * @param usage The intended usage of the image.
         * @param aspect The value specifying which aspects of an image are included in a view.
         * @param viewType The value specifying the type of the image view.
         * @param format The format and type of the texel blocks that will be contained in the image.
         * @param mipLevels The number of levels of detail available for minified sampling of the image.
         * @param arrayLayers The number of layers in the image.
         * @param extent The number of data elements in each dimension of the base level.
         * @param anisotropic If anisotropic filtering is enabled.
         * @param mipmap If mapmaps will be generated.
         */
        Texture(const std::unique_ptr<Bitmap>& bitmap,
                VkFilter filter,
                VkSamplerAddressMode addressMode,
                VkSampleCountFlagBits samples,
                VkImageLayout layout,
                VkImageUsageFlags usage,
                VkImageAspectFlags aspect,
                VkImageViewType viewType,
                VkFormat format,
                uint32_t mipLevels,
                uint32_t arrayLayers,
                const VkExtent3D& extent,
                bool anisotropic,
                bool mipmap);

        ~Texture() override = default;

        WriteDescriptorSet getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const override;
        const VkDescriptorImageInfo& getDescriptor() const { return descriptor; }

        uint32_t getMipLevels() const { return mipLevels; }
        uint32_t getArrayLevels() const { return arrayLayers; }

        //TextureType getTextureType() const { return textureType; }
        //void settTextureType(TextureType type) { textureType = type; }

        /*void transitionImage(VkImageLayout newLayout, VkCommandBuffer commandBuffer) {
            if (newLayout != layout) {
                TransitionImageLayout(commandBuffer, image, format, layout, newLayout, aspect, mipLevels, 0, arrayLayers, 0);
                layout = newLayout;
                updateDescriptor();
            }
        }*/

        /** @brief Update image descriptor from current sampler, view and image layout */
        void updateDescriptor() {
            descriptor.sampler = sampler;
            descriptor.imageView = view;
            descriptor.imageLayout = layout;
        }

    protected:
        VkDescriptorImageInfo descriptor = {};

        //TextureType textureType{ TextureType::None };
        uint32_t mipLevels{ 0 };
        uint32_t arrayLayers{ 0 };
        bool anisotropic{ false };
        bool mipmap{ false };
    };
}
