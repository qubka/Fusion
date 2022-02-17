#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    class Offscreen {
    public:
        Offscreen(Vulkan& vulkan, vk::Extent2D extent);
        Offscreen(Vulkan& vulkan, vk::Extent2D extent, std::shared_ptr<Offscreen> oldOffscreen);
        ~Offscreen();

    private:
        void init();
        void createImageViews();
        void createRenderPass();
        void createFramebuffers();

        struct FrameBufferAttachment {
            vk::Image image;
            vk::DeviceMemory memory;
            vk::ImageView view;
        };

        Vulkan& vulkan;
        vk::Extent2D extent;
        vk::Framebuffer frameBuffer;
        FrameBufferAttachment color;
        FrameBufferAttachment depth;
        vk::RenderPass renderPass;
        vk::Sampler sampler;
        vk::DescriptorImageInfo descriptor;
    };
}
