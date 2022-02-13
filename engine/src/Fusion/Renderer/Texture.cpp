#include "Texture.hpp"
#include "Image.hpp"
#include "AllocatedBuffer.hpp"

using namespace Fusion;

Texture::Texture(Vulkan& vulkan, std::string path, vk::Format format, vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) :
    vulkan{vulkan},
    path{std::move(path)},
    format{format}
{
    int channels = componentCount(format);
    Image image{getPath(), channels};

    width = static_cast<uint32_t>(image.width);
    height = static_cast<uint32_t>(image.height);

    createImage(image.pixels, magFilter, minFilter, addressMode, minmapMode);
}

Texture::Texture(Vulkan& vulkan, void* pixels, uint32_t width, uint32_t height, vk::Format format, vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) :
    vulkan{vulkan},
    width{width},
    height{height},
    format{format}
{
    createImage(pixels, magFilter, minFilter, addressMode, minmapMode);
}

Texture::~Texture() {
    vulkan.getDevice().destroySampler(sampler, nullptr);
    vulkan.getDevice().destroyImageView(view, nullptr);
    vulkan.getDevice().destroyImage(image, nullptr);
    vulkan.getDevice().freeMemory(memory, nullptr);
}

void Texture::createImage(void* pixels, vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode) {
    FE_ASSERT(width > 0 && height > 0 && "Width and height must be greater than zero!");
    FE_ASSERT(pixels && "Pixels data can be null");

    vk::DeviceSize size = width * height * componentCount(format);

    AllocatedBuffer stagingBuffer{
            vulkan,
            size,
            1,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            0
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer(pixels);

    vulkan.createImage(width, height, format,
                       vk::ImageTiling::eOptimal,
                       vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                       image, memory);

    vulkan.transitionImageLayout(image, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    vulkan.copyBufferToImage(*stagingBuffer, image, width, height, 1);
    vulkan.transitionImageLayout(image, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    vulkan.createImageView(image, format, vk::ImageAspectFlagBits::eColor, view);
    vulkan.createSampler(magFilter, minFilter, addressMode, minmapMode, sampler);
}
