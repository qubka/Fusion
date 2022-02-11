#pragma once

/*#include "Device.hpp"

namespace Fusion {
    class FUSION_API Texture {
    public:
        Texture(Device& device,
                std::string path,
                vk::Format format,
                vk::Filter magFilter = vk::Filter::eLinear,
                vk::Filter minFilter = vk::Filter::eLinear,
                vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
                vk::SamplerMipmapMode minmapMode = vk::SamplerMipmapMode::eLinear);
        Texture(Device& device,
                void* pixels,
                uint32_t width,
                uint32_t height,
                vk::Format format,
                vk::Filter magFilter = vk::Filter::eLinear,
                vk::Filter minFilter = vk::Filter::eLinear,
                vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
                vk::SamplerMipmapMode minmapMode = vk::SamplerMipmapMode::eLinear);
        ~Texture();
        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;

        const vk::Image& getImage() const { return image; };
        const vk::ImageView& getView() const { return view; };
        const vk::Sampler& getSampler() const { return sampler; };
        const std::string& getPath() const { return path; };
        uint32_t getWidth() const { return width; };
        uint32_t getHeight() const { return height; };
        vk::Format getFormat() const { return format; };

    private:
        Device& device;
        std::string path;
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;
        uint32_t width;
        uint32_t height;
        vk::Format format;
        vk::Sampler sampler;

        void createImage(void* pixels);
        void createSampler(vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode);
    };
}*/
