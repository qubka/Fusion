#pragma once

#include <volk.h>

namespace fe {
    class Swapchain;
    class LogicalDevice;
    class ImageDepth;
    class Renderpass;
    class RenderStage;
    class Image2d;

    class Framebuffers {
    public:
        Framebuffers(const LogicalDevice& logicalDevice, const Swapchain& swapchain, const RenderStage& renderStage, const Renderpass& renderPass, const ImageDepth& depthStencil,
            const glm::uvec2& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
        ~Framebuffers();
        NONCOPYABLE(Framebuffers);

        const Image2d* getAttachment(size_t index) const { return imageAttachments[index].get(); }

        const VkFramebuffer& getFramebuffer(size_t index) const { return framebuffers[index < framebuffers.size() ? index : 0]; }
        const VkFramebuffer& operator[](size_t index) const { return framebuffers[index < framebuffers.size() ? index : 0]; }

        const std::vector<std::unique_ptr<Image2d>>& getImageAttachments() const { return imageAttachments; }
        const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

    private:
        const LogicalDevice& logicalDevice;

        std::vector<std::unique_ptr<Image2d>> imageAttachments;
        std::vector<VkFramebuffer> framebuffers;
    };
}
