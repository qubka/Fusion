#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    class FUSION_API Offscreen {
    public:
        Offscreen(Vulkan& vulkan);
        ~Offscreen();
        FE_NONCOPYABLE(Offscreen);

        const VkDescriptorSet getTextureId(size_t index) const { return textures[index]; }
        const vk::Framebuffer& getFrameBuffer(size_t index) const { return frameBuffers[index]; }
        const vk::ImageView& getImageView(size_t index) const { return colors[index].view; }
        const vk::RenderPass& getRenderPass() const { return renderPass; }
        const vk::Extent2D& getExtent() const { return extent; }
        const vk::Sampler& getSampler() const { return sampler; }

        //vk::Result submitCommandBuffer(const vk::CommandBuffer& buffers);

        void createDescriptorSets(); // Call from ImGUI

    private:
        void init();
        void createOffscreen();
        void createRenderPass();
        void createFramebuffers();

        Vulkan& vulkan;
        vk::Extent2D extent;

        struct FrameBufferAttachment {
            vk::Image image;
            vk::ImageView view;
            vk::DeviceMemory memory;
        };

        std::vector<VkDescriptorSet> textures; /* ImTextureID */

        std::vector<FrameBufferAttachment> colors;
        vk::Format colorFormat;

        FrameBufferAttachment depth;
        vk::Format depthFormat;

        std::vector<vk::Framebuffer> frameBuffers;
        vk::RenderPass renderPass;
        vk::Sampler sampler;

        //void* textureID;
    };
}