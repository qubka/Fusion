#pragma once

#include <volk/volk.h>

namespace fe {
    class Swapchain;
    class LogicalDevice;
    class TextureDepth;
    class Renderpass;
    class RenderStage;
    class Texture2d;

    class Framebuffers {
    public:
        Framebuffers(const LogicalDevice& logicalDevice, const Swapchain& swapchain, const RenderStage& renderStage, const Renderpass& renderPass, const TextureDepth& depthStencil,
            const glm::uvec2& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
        ~Framebuffers();
        NONCOPYABLE(Framebuffers);

        const Texture2d* getAttachment(size_t index) const { return imageAttachments[index].get(); }

        const VkFramebuffer& getFramebuffer(size_t index) const { return index < framebuffers.size() ? framebuffers[index] : framebuffers.front(); }
        const VkFramebuffer& operator[](size_t index) const { return index < framebuffers.size() ? framebuffers[index] : framebuffers.front(); }

        const std::vector<std::unique_ptr<Texture2d>>& getImageAttachments() const { return imageAttachments; }
        const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

    private:
        const LogicalDevice& logicalDevice;

        std::vector<std::unique_ptr<Texture2d>> imageAttachments;
        std::vector<VkFramebuffer> framebuffers;
    };
}
