#include "Offscreen.hpp"

using namespace Fusion;

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

void {
    // Find a suitable depth format
    VkFormat fbDepthFormat;
    VkBool32 validDepthFormat = vks::tools::getSupportedDepthFormat(physicalDevice, &fbDepthFormat);
    assert(validDepthFormat);

    // Color attachment
    VkImageCreateInfo image = vks::initializers::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = FB_COLOR_FORMAT;
    image.extent.width = offscreenPass.width;
    image.extent.height = offscreenPass.height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    // We will sample directly from the color attachment
    image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    VkMemoryAllocateInfo memAlloc = vks::initializers::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    VK_CHECK_RESULT(vkCreateImage(device, &image, nullptr, &offscreenPass.color.image));

    vkGetImageMemoryRequirements(device, offscreenPass.color.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;

    memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &offscreenPass.color.mem));
    VK_CHECK_RESULT(vkBindImageMemory(device, offscreenPass.color.image, offscreenPass.color.mem, 0));

    VkImageViewCreateInfo colorImageView = vks::initializers::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = FB_COLOR_FORMAT;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = offscreenPass.color.image;
    VK_CHECK_RESULT(vkCreateImageView(device, &colorImageView, nullptr, &offscreenPass.color.view));

    // Create sampler to sample from the attachment in the fragment shader
    VkSamplerCreateInfo samplerInfo = vks::initializers::samplerCreateInfo();
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = samplerInfo.addressModeU;
    samplerInfo.addressModeW = samplerInfo.addressModeU;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &offscreenPass.sampler));

    // Depth stencil attachment
    image.format = fbDepthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VK_CHECK_RESULT(vkCreateImage(device, &image, nullptr, &offscreenPass.depth.image));
    vkGetImageMemoryRequirements(device, offscreenPass.depth.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &offscreenPass.depth.mem));
    VK_CHECK_RESULT(vkBindImageMemory(device, offscreenPass.depth.image, offscreenPass.depth.mem, 0));

    VkImageViewCreateInfo depthStencilView = vks::initializers::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = fbDepthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = offscreenPass.depth.image;
    VK_CHECK_RESULT(vkCreateImageView(device, &depthStencilView, nullptr, &offscreenPass.depth.view));
}

// The color attachment of this framebuffer will then be used to sample from in the fragment shader of the final pass
void Offscreen::createRenderPass() {
    // Create a separate render pass for the offscreen rendering as it may differ from the one used for scene rendering

    // Color attachment
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = FB_COLOR_FORMAT;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    // Depth attachment
    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = fbDepthFormat;
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

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

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

void {
    std::array<vk::ImageView, 2> attachments = {color.view, depth.view};

    vk::FramebufferCreateInfo framebufferInfo{};
    fbufCreateInfo.renderPass = offscreenPass.renderPass;
    fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    fbufCreateInfo.pAttachments = attachments.data();
    fbufCreateInfo.width = offscreenPass.width;
    fbufCreateInfo.height = offscreenPass.height;
    fbufCreateInfo.layers = 1;

    auto result = vulkan.getDevice().createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create framebuffer!");

    // Fill a descriptor for later use in a descriptor set
    descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptor.imageView = offscreenPass.color.view;
    descriptor.sampler = offscreenPass.sampler;
}