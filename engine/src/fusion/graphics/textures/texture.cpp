#include "texture.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/buffers/buffer.hpp"

using namespace fe;

Texture::Texture(const fs::path& filepath, VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples,
                 VkImageLayout layout, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageViewType viewType,
                 VkFormat format, uint32_t mipLevels, uint32_t arrayLayers, const VkExtent3D& extent, bool anisotropic, bool mipmap)
        : Image{filter, addressMode, samples, layout, usage, aspect, viewType, format, extent}
        , mipLevels{mipLevels}
        , arrayLayers{arrayLayers}
        , anisotropic{anisotropic}
        , mipmap{mipmap} {
    path = filepath;
}

Texture::Texture(VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, VkImageLayout layout,
                 VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageViewType viewType, VkFormat format, uint32_t mipLevels, uint32_t arrayLayers,
                 const VkExtent3D& extent, bool anisotropic, bool mipmap)
        : Image{filter, addressMode, samples, layout, usage, aspect, viewType, format, extent}
        , mipLevels{mipLevels}
        , arrayLayers{arrayLayers}
        , anisotropic{anisotropic}
        , mipmap{mipmap} {
    if (extent.width == 0 || extent.width == 0)
        throw std::runtime_error("Width or height is empty");

    mipLevels = mipmap ? GetMipLevels(extent) : 1;

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);

    if (mipmap) {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        if (aspect == VK_IMAGE_ASPECT_DEPTH_BIT && HasStencil(format))
            aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    updateDescriptor();
}

Texture::Texture(const std::unique_ptr<Bitmap>& bitmap, VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples,
                 VkImageLayout layout, VkImageUsageFlags usage,  VkImageAspectFlags aspect, VkImageViewType viewType, VkFormat format,
                 uint32_t mipLevels, uint32_t arrayLayers, const VkExtent3D& extent, bool anisotropic, bool mipmap)
        : Image{filter, addressMode, samples, layout, usage, aspect, viewType, format, extent}
        , mipLevels{mipLevels}
        , arrayLayers{arrayLayers}
        , anisotropic{anisotropic}
        , mipmap{mipmap} {
    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    mipLevels = mipmap ? GetMipLevels(extent) : 1;

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);
    TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);

    Buffer bufferStaging{bitmap->getLength() * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bitmap->getData<void>()};
    CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        if (aspect == VK_IMAGE_ASPECT_DEPTH_BIT && HasStencil(format))
            aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    updateDescriptor();
}

WriteDescriptorSet Texture::getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const {
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;
    //descriptorWrite.pImageInfo = &imageInfo;
    return {descriptorWrite, descriptor};
}

