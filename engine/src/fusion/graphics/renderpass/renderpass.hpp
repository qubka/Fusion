#pragma once

namespace fe {
    class LogicalDevice;
    class TextureDepth;
    class RenderStage;

    class Renderpass {
    public:
        class SubpassDescription {
        public:
            SubpassDescription(VkPipelineBindPoint bindPoint, std::vector<VkAttachmentReference>&& colorAttachment, const std::optional<uint32_t>& depthAttachment, bool resolveAttachment) : colorAttachments{std::move(colorAttachment)} {
                subpassDescription.pipelineBindPoint = bindPoint;
                subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
                subpassDescription.pColorAttachments = colorAttachments.data();

                if (resolveAttachment) {
                    subpassDescription.colorAttachmentCount--;
                    subpassDescription.pResolveAttachments = &colorAttachments.back();
                }

                if (depthAttachment) {
                    depthStencilAttachment.attachment = *depthAttachment;
                    depthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    subpassDescription.pDepthStencilAttachment = &depthStencilAttachment;
                }
            }
            NONCOPYABLE(SubpassDescription);

            operator const VkSubpassDescription&() const { return subpassDescription; }
            const VkSubpassDescription& getSubpassDescription() const { return subpassDescription; }

        private:
            std::vector<VkAttachmentReference> colorAttachments;

            VkSubpassDescription subpassDescription = {};
            VkAttachmentReference depthStencilAttachment = {};
        };

        Renderpass(const LogicalDevice& logicalDevice, const RenderStage& renderStage, VkFormat depthFormat, VkFormat surfaceFormat, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
        ~Renderpass();

        operator bool() const { return renderpass != VK_NULL_HANDLE; }
        operator const VkRenderPass&() const { return renderpass; }
        const VkRenderPass& getRenderpass() const { return renderpass; }

    private:
        const LogicalDevice& logicalDevice;

        VkRenderPass renderpass{ VK_NULL_HANDLE };
    };
}
