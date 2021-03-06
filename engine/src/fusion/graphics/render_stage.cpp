#include "render_stage.hpp"
#include "graphics.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/graphics/textures/texture_depth.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/graphics/renderpass/framebuffers.hpp"
#include "fusion/graphics/renderpass/renderpass.hpp"
#include "fusion/graphics/renderpass/swapchain.hpp"
#include "fusion/utils/date_time.hpp"

using namespace fe;

RenderStage::RenderStage(std::vector<Attachment> images, std::vector<SubpassType> subpasses, const Viewport& viewport)
        : attachments{std::move(images)}
        , subpasses{std::move(subpasses)}
        , viewport{viewport}
        , subpassAttachmentCount(this->subpasses.size())
        , subpassMultisampled(this->subpasses.size()) {
	for (const auto& image : attachments) {
		VkClearValue clearValue = {};

		switch (image.type) {
            case Attachment::Type::Image:
                clearValue.color = vku::clearColor(image.clearColor);

                for (const auto& subpass : this->subpasses) {
                    if (const auto& subpassBindings = subpass.attachmentBindings; std::find(subpassBindings.begin(), subpassBindings.end(), image.binding) != subpassBindings.end()) {
                        subpassAttachmentCount[subpass.binding]++;

                        if (image.multisampled)
                            subpassMultisampled[subpass.binding] = true;
                    }
                }

                break;
            case Attachment::Type::Depth:
                clearValue.depthStencil = { 1.0f, 0 };
                depthAttachment = image;
                break;
            case Attachment::Type::Swapchain:
                clearValue.color = vku::clearColor(image.clearColor);
                swapchainAttachment = image;
                break;
        }

		clearValues.push_back(clearValue);
	}
}

void RenderStage::update(size_t id, const Swapchain& swapchain) {
	auto lastRenderArea = renderArea;

	renderArea.offset = viewport.offset;

	if (viewport.size)
		renderArea.extent = viewport.scale * glm::vec2{*viewport.size};
	else
		renderArea.extent = viewport.scale * glm::vec2{ vku::extent2D_cast(swapchain.getExtent()) };

	renderArea.extent += renderArea.offset;

	outOfDate = renderArea != lastRenderArea;
}

void RenderStage::rebuild(size_t id, const Swapchain& swapchain) {
#if FUSION_DEBUG
	auto debugStart = DateTime::Now();
#endif

	update(id, swapchain);

    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
	const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto msaaSamples = physicalDevice.getMsaaSamples();

	if (depthAttachment)
		depthStencil = std::make_unique<TextureDepth>(renderArea.extent, depthAttachment->multisampled ? msaaSamples : VK_SAMPLE_COUNT_1_BIT);

	if (!renderpass)
		renderpass = std::make_unique<Renderpass>(logicalDevice, *this, depthStencil ? depthStencil->getFormat() : VK_FORMAT_UNDEFINED, swapchain.getSurfaceFormat().format, msaaSamples);

	framebuffers = std::make_unique<Framebuffers>(logicalDevice, swapchain, *this, *renderpass, *depthStencil, renderArea.extent, msaaSamples);
	outOfDate = false;

	descriptors.clear();
	auto where = descriptors.end();

	for (const auto& image : attachments) {
		if (image.type == Attachment::Type::Depth)
			where = descriptors.insert(where, { image.name, depthStencil.get() });
		else
			where = descriptors.insert(where, { image.name, framebuffers->getAttachment(image.binding) });
	}

#if FUSION_DEBUG
	LOG_DEBUG << "Render Stage created in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

std::optional<Attachment> RenderStage::getAttachment(const std::string& name) const {
	auto it = std::find_if(attachments.begin(), attachments.end(), [name](const Attachment& a) {
		return a.name == name;
	});
	if (it != attachments.end())
		return *it;
	return std::nullopt;
}

std::optional<Attachment> RenderStage::getAttachment(uint32_t binding) const {
	auto it = std::find_if(attachments.begin(), attachments.end(), [binding](const Attachment& a) {
		return a.binding == binding;
	});
	if (it != attachments.end())
		return *it;
	return std::nullopt;
}

const Descriptor* RenderStage::getDescriptor(const std::string& name) const {
    if (auto it = descriptors.find(name); it != descriptors.end())
        return it->second;
    return nullptr;
}

const VkFramebuffer& RenderStage::getActiveFramebuffer(uint32_t activeSwapchainImage) const {
	return framebuffers->getFramebuffer(activeSwapchainImage);
}

bool RenderStage::setViewport(const Viewport& port) {
    if (viewport == port)
        return false;

    viewport = port;
    return true;
}
