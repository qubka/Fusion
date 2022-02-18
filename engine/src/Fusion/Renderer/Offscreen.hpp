#pragma once

#include "Vulkan.hpp"

/*namespace Fusion {
    class Offscreen {
    public:
        Offscreen(Vulkan& vulkan, vk::Extent2D extent);
        Offscreen(Vulkan& vulkan, vk::Extent2D extent, std::shared_ptr<Offscreen> oldOffscreen);
        ~Offscreen();
        FE_NONCOPYABLE(Offscreen);

        const vk::Framebuffer& getFrameBuffer() const { return frameBuffer; }
        const vk::RenderPass& getRenderPass() const { return renderPass; }
        const vk::ImageView& getImageView(size_t index) const { return swapChainImageViews[index]; }
        const vk::Format& getSwapChainImageFormat() const { return swapChainImageFormat; }
        const vk::Extent2D& getSwapChainExtent() const { return swapChainExtent; }
        size_t imageCount() const { return swapChainImages.size(); }

    private:
        void init();
        void createOffscreen();
        void createRenderPass();
        void createFramebuffers();

        struct FrameBufferAttachment {
            vk::Image image;
            vk::DeviceMemory memory;
            vk::ImageView view;
            vk::Format format;
        };

        Vulkan& vulkan;
        vk::Extent2D extent;
        vk::Framebuffer frameBuffer;
        FrameBufferAttachment color;
        FrameBufferAttachment depth;
        vk::RenderPass renderPass;
        vk::Sampler sampler;
    };
}*/
