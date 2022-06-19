#include "renderstage.hpp"

using namespace fe;
using namespace vkx;

RenderStage::RenderStage(std::vector<vkx::Attachment> images, std::vector<vkx::SubpassType> subpasses, const Viewport& viewport)
    : attachments{std::move(images)}
    , subpasses{std::move(subpasses)}
    , viewport{viewport}
    , subpassAttachmentCount(this->subpasses.size()) // resize
    , subpassMultisampled(this->subpasses.size()) // resize
{
    clearValues.reserve(attachments.size());
    for (const auto& attachment : attachments) {
        vk::ClearValue clearValue{};

        switch (attachment.type) {
            case Attachment::Type::Image:
                clearValue.color = attachment.clearColour;

                for (const auto& subpass : this->subpasses) {
                    if (const auto& subpassBindings = subpass.attachmentBindings; std::find(subpassBindings.begin(), subpassBindings.end(), attachment.binding) != subpassBindings.end()) {
                        subpassAttachmentCount[subpass.binding]++;

                        if (attachment.multisampled)
                            subpassMultisampled[subpass.binding] = true;
                    }
                }
                break;
            case Attachment::Type::Depth:
                clearValue.depthStencil = vk::ClearDepthStencilValue{1.0f, 0};
                depthAttachment = attachment;
                break;
            case Attachment::Type::Swapchain:
                clearValue.color = attachment.clearColour;
                swapchainAttachment = attachment;
                break;
        }

        clearValues.emplace_back(clearValue);
    }
}

void RenderStage::rebuild(const vkx::Swapchain& swapchain) {
    update();

    if (depthAttachment) {
        depthStencil = std::make_unique<vkx::Image>(renderArea.extent, depthAttachment->multisampled ? msaaSamples : VK_SAMPLE_COUNT_1_BIT);
    }

    if (!renderpass) {

    }

    framebuffers = std::make_unique<Framebuffers>(context, renderArea.extent, attachments, *renderpass, swapchain, *depthStencil);
    outOfDate = false;

    descriptors.clear();
    auto where = descriptors.end();

    for (const auto& attachment : attachments) {
        if (attachment.type == vkx::Attachment::Type::Depth)
            where = descriptors.insert(where, {attachment.name, depthStencil.get()});
        else
            where = descriptors.insert(where, {attachment.name, framebuffers.imageAttachments[attachment.binding]});
    }
}

void RenderStage::update() {
    auto lastRenderArea = renderArea;

    renderArea.offset = viewport.offset;

    if (viewport.size)
        renderArea.extent = viewport.scale * glm::vec2{*viewport.size};
    else
        renderArea.extent = viewport.scale * glm::vec2{1012, 720};//Windows::Get()->GetWindow(0)->GetSize();

    renderArea.aspectRatio = (static_cast<float>(renderArea.extent.x) / static_cast<float>(renderArea.extent.y));
    renderArea.extent += renderArea.offset;

    outOfDate = renderArea != lastRenderArea;
}

std::optional<vkx::Attachment> RenderStage::getAttachment(const std::string& name) const {
    for (const auto& attachment : attachments) {
        if (attachment.name == name) {
            return attachment;
        }
    }
    return std::nullopt;
}

std::optional<vkx::Attachment> RenderStage::getAttachment(uint32_t binding) const {
    for (const auto& attachment : attachments) {
        if (attachment.binding == binding) {
            return attachment;
        }
    }
    return std::nullopt;
}

/*const Descriptor* RenderStage::getDescriptor(const std::string &name) const {
    if (auto it = descriptors.find(name); it != descriptors.end()) {
        return it->second;
    }
    return nullptr;
}*/

const vk::Framebuffer& RenderStage::getActiveFramebuffer(uint32_t activeSwapchainImage) const {
    if (activeSwapchainImage > framebuffers.size())
        return framebuffers[0];
    return framebuffers[activeSwapchainImage];
}
