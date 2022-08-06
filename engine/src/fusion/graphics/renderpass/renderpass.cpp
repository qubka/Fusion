#include "renderpass.hpp"
#include "fusion/graphics/graphics.hpp"

using namespace fe;

Renderpass::Renderpass(const LogicalDevice& logicalDevice, const RenderStage& renderStage, VkFormat depthFormat, VkFormat surfaceFormat, VkSampleCountFlagBits samples) : logicalDevice{logicalDevice} {
	// Creates the renderpasses attachment descriptions,
	std::vector<VkAttachmentDescription> attachmentDescriptions;
    attachmentDescriptions.reserve(renderStage.getAttachments().size());

    auto lastBinding = renderStage.getAttachments().back().binding;

	for (const auto& attachment : renderStage.getAttachments()) {
		VkAttachmentDescription attachmentDescription = {};
		attachmentDescription.samples = attachment.multisampled && attachment.binding != lastBinding ? samples : VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear at beginning of the render pass.
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // // The image can be read from so it's important to store the attachment results
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We don't care about initial layout of the attachment.

        switch (attachment.type) {
            case Attachment::Type::Image:
                attachmentDescription.finalLayout = attachment.layout;
                attachmentDescription.format = attachment.format != VK_FORMAT_UNDEFINED ? attachment.format : surfaceFormat;
                break;
            case Attachment::Type::Depth:
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachmentDescription.format = depthFormat;
                break;
            case Attachment::Type::Swapchain:
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                attachmentDescription.format = surfaceFormat;
                break;
		}

		attachmentDescriptions.push_back(attachmentDescription);
	}

	// Creates each subpass and its dependencies.
	std::vector<std::unique_ptr<SubpassDescription>> subpasses;
	std::vector<VkSubpassDependency> dependencies;

	for (const auto& subpassType : renderStage.getSubpasses()) {
		// Attachments.
		std::vector<VkAttachmentReference> subpassColorAttachments;

		std::optional<uint32_t> depthAttachment;
        bool resolveAttachment = false;

        for (const auto& attachmentBinding : subpassType.attachmentBindings) {
			auto attachment = renderStage.getAttachment(attachmentBinding);

			if (!attachment) {
				LOG_ERROR << "Failed to find a renderpass attachment bound to: " << attachmentBinding;
				continue;
			}

			if (attachment->type == Attachment::Type::Depth) {
				depthAttachment = attachment->binding;
				continue;
			}

			VkAttachmentReference attachmentReference = {};
			attachmentReference.attachment = attachment->binding;
            attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			subpassColorAttachments.push_back(attachmentReference);

            resolveAttachment = attachment->multisampled; // last attachment as resolve
		}

		// Subpass description.
		subpasses.push_back(std::make_unique<SubpassDescription>(VK_PIPELINE_BIND_POINT_GRAPHICS, std::move(subpassColorAttachments), depthAttachment, resolveAttachment));

		// Subpass dependencies.
		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if (subpassType.binding == renderStage.getSubpasses().size()) {
			subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		} else {
			subpassDependency.dstSubpass = subpassType.binding;
		}

		if (subpassType.binding == 0) {
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		} else {
			subpassDependency.srcSubpass = subpassType.binding - 1;
		}

		dependencies.push_back(subpassDependency);
	}

	std::vector<VkSubpassDescription> subpassDescriptions;
	subpassDescriptions.reserve(subpasses.size());

	for (const auto& subpass : subpasses) {
		subpassDescriptions.push_back(*subpass);
	}

	// Creates the render pass.
	VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
	renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCreateInfo.pDependencies = dependencies.data();
	VK_CHECK(vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderpass));
}

Renderpass::~Renderpass() {
	vkDestroyRenderPass(logicalDevice, renderpass, nullptr);
}
