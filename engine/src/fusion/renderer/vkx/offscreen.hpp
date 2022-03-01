#pragma once

#include "context.hpp"
#include "framebuffer.hpp"

namespace vkx {
class Offscreen {
public:
    Offscreen(const vkx::Context& context)
            : context{ context } {}

    const vkx::Context& context;
    vk::RenderPass renderPass;
    vk::CommandBuffer commandBuffer;
    vk::Semaphore renderComplete;
    glm::uvec2 size{ 0 };
    std::vector<vk::Format> colorFormats{ vk::Format::eB8G8R8A8Unorm };
    // This value is chosen as an invalid default that signals that the code should pick a specific depth buffer
    // Alternative, you can set this to undefined to explicitly declare you want no depth buffer.
    vk::Format depthFormat{ vk::Format::eR8Uscaled };
    std::vector<vkx::Framebuffer> framebuffers{ 1 };
    vk::ImageUsageFlags attachmentUsage{ vk::ImageUsageFlagBits::eSampled };
    vk::ImageUsageFlags depthAttachmentUsage;
    vk::ImageLayout colorFinalLayout{ vk::ImageLayout::eShaderReadOnlyOptimal };
    vk::ImageLayout depthFinalLayout{ vk::ImageLayout::eDepthStencilAttachmentOptimal };

    bool active{ true };

    void create() {
        assert(!colorFormats.empty());
        assert(size != glm::uvec2());

        if (depthFormat == vk::Format::eR8Uscaled) {
            depthFormat = context.getSupportedDepthFormat();
        }

        commandBuffer = context.allocateCommandBuffers(1)[0];
        renderComplete = context.device.createSemaphore({});

        if (!renderPass) {
            createRenderPass();
        }

        for (auto& framebuffer: framebuffers) {
            framebuffer.create(context, size, colorFormats, depthFormat, renderPass, attachmentUsage,
                               depthAttachmentUsage);
        }
        createSampler();
    }

    void destroy() {
        for (auto& framebuffer: framebuffers) {
            framebuffer.destroy();
        }
        framebuffers.clear();
        context.device.freeCommandBuffers(context.getCommandPool(), commandBuffer);
        context.device.destroyRenderPass(renderPass);
        context.device.destroySemaphore(renderComplete);
    }

protected:
    void createSampler() {
        // Create sampler
        vk::SamplerCreateInfo sampler;
        sampler.magFilter = vk::Filter::eLinear;
        sampler.minFilter = vk::Filter::eLinear;
        sampler.mipmapMode = vk::SamplerMipmapMode::eLinear;
        sampler.addressModeU = vk::SamplerAddressMode::eClampToEdge;
        sampler.addressModeV = sampler.addressModeU;
        sampler.addressModeW = sampler.addressModeU;
        sampler.mipLodBias = 0.0f;
        sampler.maxAnisotropy = 0;
        sampler.compareOp = vk::CompareOp::eNever;
        sampler.minLod = 0.0f;
        sampler.maxLod = 0.0f;
        sampler.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        for (auto& framebuffer: framebuffers) {
            if (attachmentUsage | vk::ImageUsageFlagBits::eSampled) {
                for (auto& color: framebuffer.colors) {
                    color.sampler = context.device.createSampler(sampler);
                }
            }
            if (depthAttachmentUsage | vk::ImageUsageFlagBits::eSampled) {
                framebuffer.depth.sampler = context.device.createSampler(sampler);
            }
        }
    }

    void createRenderPass() {
        vk::SubpassDescription subpass;
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

        std::vector<vk::AttachmentDescription> attachments;
        std::vector<vk::AttachmentReference> colorAttachmentReferences;
        attachments.resize(colorFormats.size());
        colorAttachmentReferences.resize(attachments.size());
        // Color attachment
        for (uint32_t i = 0; i < attachments.size(); ++i) {
            attachments[i].format = colorFormats[i];
            attachments[i].loadOp = vk::AttachmentLoadOp::eClear;
            attachments[i].storeOp =
                    colorFinalLayout == vk::ImageLayout::eColorAttachmentOptimal ? vk::AttachmentStoreOp::eDontCare
                                                                                 : vk::AttachmentStoreOp::eStore;
            attachments[i].initialLayout = vk::ImageLayout::eUndefined;
            attachments[i].finalLayout = colorFinalLayout;

            vk::AttachmentReference& attachmentReference = colorAttachmentReferences[i];
            attachmentReference.attachment = i;
            attachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

            subpass.colorAttachmentCount = (uint32_t) colorAttachmentReferences.size();
            subpass.pColorAttachments = colorAttachmentReferences.data();
        }

        // Do we have a depth format?
        vk::AttachmentReference depthAttachmentReference;
        if (depthFormat != vk::Format::eUndefined) {
            vk::AttachmentDescription depthAttachment;
            depthAttachment.format = depthFormat;
            depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            // We might be using the depth attacment for something, so preserve it if it's final layout is not undefined
            depthAttachment.storeOp = depthFinalLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal
                                      ? vk::AttachmentStoreOp::eDontCare : vk::AttachmentStoreOp::eStore;
            depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
            depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout = depthFinalLayout;
            attachments.push_back(depthAttachment);
            depthAttachmentReference.attachment = (uint32_t) attachments.size() - 1;
            depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            subpass.pDepthStencilAttachment = &depthAttachmentReference;
        }

        std::vector<vk::SubpassDependency> subpassDependencies;
        {
            if ((colorFinalLayout != vk::ImageLayout::eColorAttachmentOptimal) &&
                (colorFinalLayout != vk::ImageLayout::eUndefined)) {
                // Implicit transition
                vk::SubpassDependency dependency;
                dependency.srcSubpass = 0;
                dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

                dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstAccessMask = vkx::util::accessFlagsForLayout(colorFinalLayout);
                dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
                subpassDependencies.push_back(dependency);
            }

            if ((depthFinalLayout != vk::ImageLayout::eDepthStencilAttachmentOptimal) &&
                (depthFinalLayout != vk::ImageLayout::eUndefined)) {
                // Implicit transition
                vk::SubpassDependency dependency;
                dependency.srcSubpass = 0;
                dependency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                dependency.srcStageMask = vk::PipelineStageFlagBits::eLateFragmentTests;

                dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstAccessMask = vkx::util::accessFlagsForLayout(depthFinalLayout);
                dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
                subpassDependencies.push_back(dependency);
            }
        }

        if (renderPass) {
            context.device.destroyRenderPass(renderPass);
        }

        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.attachmentCount = (uint32_t) attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = (uint32_t) subpassDependencies.size();
        renderPassInfo.pDependencies = subpassDependencies.data();
        renderPass = context.device.createRenderPass(renderPassInfo);
    }
};
} // namespace vkx