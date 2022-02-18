#include "Offscreen.hpp"

/*using namespace Fusion;

Offscreen::Offscreen(Vulkan& vulkan, vk::Extent2D windowExtent) : vulkan{vulkan}, extent{extent} {

}

Offscreen::~Offscreen() {
    // Color attachment
    vulkan.getDevice().destroyImageView(color.view, nullptr);
    vulkan.getDevice().destroyImage(color.image, nullptr);
    vulkan.getDevice().freeMemory(color.memory, nullptr);

    // Depth attachment
    vulkan.getDevice().destroyImageView(depth.view, nullptr);
    vulkan.getDevice().destroyImage(depth.image, nullptr);
    vulkan.getDevice().freeMemory(depth.memory, nullptr);

    vulkan.getDevice().destroySampler(sampler, nullptr);
    vulkan.getDevice().destroyRenderPass(renderPass, nullptr);
    vulkan.getDevice().destroyFramebuffer( frameBuffer, nullptr);
}

void Offscreen::init() {
    createOffscreen();
    createRenderPass();
    createFramebuffers();
}

void Offscreen::createOffscreen() {
    // Find a suitable color and depth format
    color.format = vk::Format::eR8G8B8A8Unorm;
    depth.format = vulkan.findDepthFormat();

    // Color attachment
    vulkan.createImage(
            extent.width,
            extent.height,
            color.format,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            color.image,
            color.memory);

    vulkan.createImageView(color.image, color.format, vk::ImageAspectFlagBits::eColor, color.view);

    // Create sampler to sample from the attachment in the fragment shader
    vulkan.createSampler(
            vk::Filter::eLinear,
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eClampToEdge,
            vk::SamplerMipmapMode::eLinear,
            vk::BorderColor::eFloatOpaqueWhite,
            sampler);

    // Depth stencil attachment
    vulkan.createImage(
            extent.width,
            extent.height,
            depth.format,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            depth.image,
            depth.memory);

    vulkan.createImageView(depth.image, depth.format, vk::ImageAspectFlagBits::eDepth, depth.view);
}

// The color attachment of this framebuffer will then be used to sample from in the fragment shader of the final pass
void Offscreen::createRenderPass() {
    // Create a separate render pass for the offscreen rendering as it may differ from the one used for scene rendering

    // Color attachment
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = color.format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    // Depth attachment
    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = depth.format;
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorReference = { 0, vk::ImageLayout::eColorAttachmentOptimal };
    vk::AttachmentReference depthReference = { 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

    vk::SubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // Create the actual renderpass
    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    auto result = vulkan.getDevice().createRenderPass(&renderPassInfo, nullptr, &renderPass);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create render pass!");
}

void Offscreen::createFramebuffers() {
    std::array<vk::ImageView, 2> attachments = {color.view, depth.view};

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    auto result = vulkan.getDevice().createFramebuffer(&framebufferInfo, nullptr, &frameBuffer);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create framebuffer!");
}*/