#include "framebuffers.hpp"

#include "fusion/graphics/render_stage.hpp"
#include "fusion/graphics/textures/texture_depth.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/graphics/renderpass/renderpass.hpp"
#include "fusion/graphics/renderpass/swapchain.hpp"
#include "fusion/graphics/graphics.hpp"

using namespace fe;

Framebuffers::Framebuffers(const LogicalDevice& logicalDevice, const Swapchain& swapchain, const RenderStage& renderStage, const Renderpass& renderPass, const TextureDepth& depthStencil, const glm::uvec2& extent, VkSampleCountFlagBits samples) :
	logicalDevice{logicalDevice} {
	for (const auto& attachment : renderStage.getAttachments()) {
        switch (attachment.type) {
            case Attachment::Type::Image:
                imageAttachments.push_back(std::make_unique<Texture2d>(extent, attachment.format != VK_FORMAT_UNDEFINED ? attachment.format : swapchain.getSurfaceFormat().format,
                   attachment.layout, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, attachment.multisampled ? samples : VK_SAMPLE_COUNT_1_BIT));
                break;
            case Attachment::Type::Depth:
                imageAttachments.emplace_back(nullptr);
                break;
            case Attachment::Type::Swapchain:
                imageAttachments.emplace_back(nullptr);
                break;
        }
	}

	framebuffers.resize(swapchain.getImageCount());

	for (uint32_t i = 0; i < swapchain.getImageCount(); i++) {
		std::vector<VkImageView> attachments;
		attachments.reserve(renderStage.getAttachments().size());

		for (const auto& attachment : renderStage.getAttachments()) {
			switch (attachment.type) {
                case Attachment::Type::Image:
                    attachments.push_back(getAttachment(attachment.binding)->getView());
                    break;
                case Attachment::Type::Depth:
                    attachments.push_back(depthStencil.getView());
                    break;
                case Attachment::Type::Swapchain:
                    attachments.push_back(swapchain.getImageViews().at(i));
                    break;
			}
		}

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = extent.x;
		framebufferCreateInfo.height = extent.y;
		framebufferCreateInfo.layers = 1;
		VK_CHECK(vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &framebuffers[i]));
	}
}

Framebuffers::~Framebuffers() {
	for (const auto& framebuffer : framebuffers)
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
}
