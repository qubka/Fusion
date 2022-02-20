#pragma once

#include "context.hpp"
#include "framebuffer.hpp"

namespace vkx {

class Offscreen {
public:
    Offscreen(const vkx::Context& context)
            : context{context} {}

    const vkx::Context& context;
    vk::RenderPass renderPass;
    vk::CommandBuffer cmdBuffer;
    vk::Semaphore renderComplete;
    glm::uvec2 size { 0 };
    std::vector<vk::Format> colorFormats{ vk::Format::eB8G8R8A8Unorm };
    // This value is chosen as an invalid default that signals that the code should pick a specific depth buffer
    // Alternative, you can set this to undefined to explicitly declare you want no depth buffer.
    vk::Format depthFormat = vk::Format::eR8Uscaled;
    std::vector<vkx::Framebuffer> framebuffers{ 1 };
    vk::ImageUsageFlags attachmentUsage{ vk::ImageUsageFlagBits::eSampled };
    vk::ImageUsageFlags depthAttachmentUsage;
    vk::ImageLayout colorFinalLayout{ vk::ImageLayout::eShaderReadOnlyOptimal };
    vk::ImageLayout depthFinalLayout{ vk::ImageLayout::eDepthStencilAttachmentOptimal };

    void destroy() {
        for (auto& framebuffer : framebuffers) {
            framebuffer.destroy();
        }
        framebuffers.clear();
        context.device.freeCommandBuffers(context.getCommandPool(), cmdBuffer);
        context.device.destroyRenderPass(renderPass);
        context.device.destroySemaphore(renderComplete);
    }
};

struct OffscreenRenderer {
    const vkx::Context& context;
    const vk::Device& device;
    const vk::Queue& queue;
    vk::CommandPool cmdPool;
    vk::RenderPass renderPass;
    glm::uvec2 framebufferSize;
    std::vector<vk::Format> colorFormats;
    // This value is chosen as an invalid default that signals that the code should pick a specific depth buffer
    // Alternative, you can set this to undefined to explicitly declare you want no depth buffer.
    vk::Format depthFormat = vk::Format::eR8Uscaled;
    struct {
        vk::Semaphore renderStart;
        vk::Semaphore renderComplete;
    } semaphores;
    vkx::Framebuffer framebuffer;
    // Typically the offscreen render results will either be used for a blit operation or a shader read operation
    // so the final color layout is usually either TransferSrcOptimal or ShaderReadOptimal
    vk::ImageLayout colorFinalLayout{ vk::ImageLayout::eTransferSrcOptimal };
    vk::ImageLayout depthFinalLayout{ vk::ImageLayout::eUndefined };
    vk::SubmitInfo submitInfo;
    vk::ImageUsageFlags attachmentUsage{ vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eInputAttachment };
    vk::DescriptorPool descriptorPool;

    OffscreenRenderer(const vkx::Context& context)
        : context(context)
        , device(context.device)
        , queue(context.queue)
        , colorFormats({ vk::Format::eB8G8R8A8Unorm }) {}

    void destroy() {
        queue.waitIdle();
        device.waitIdle();
        framebuffer.destroy();
        device.destroyRenderPass(renderPass);
        device.destroySemaphore(semaphores.renderComplete);
        device.destroySemaphore(semaphores.renderStart);
    }

    void prepare() {
        cmdPool = context.getCommandPool();
        semaphores.renderComplete = device.createSemaphore(vk::SemaphoreCreateInfo());
        semaphores.renderStart = device.createSemaphore(vk::SemaphoreCreateInfo());
        prepareRenderPass();
        prepareFramebuffer();
        prepareSampler();
    }

    void prepareFramebuffer() {
        assert(framebufferSize != glm::uvec2{});
        depthFormat = context.getSupportedDepthFormat();
        framebuffer.create(context, framebufferSize, colorFormats, depthFormat, renderPass, attachmentUsage);
    }

    void prepareSampler() {
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
        for (auto& color : framebuffer.colors) {
            color.sampler = device.createSampler(sampler);
        }
    }

    void prepareRenderPass() {
        std::vector<vk::AttachmentDescription> attachments;
        std::vector<vk::AttachmentReference> colorAttachmentReferences;
        attachments.resize(colorFormats.size());
        colorAttachmentReferences.resize(attachments.size());
        // Color attachment
        for (size_t i = 0; i < attachments.size(); ++i) {
            attachments[i].format = colorFormats[i];
            attachments[i].loadOp = vk::AttachmentLoadOp::eClear;
            attachments[i].storeOp = vk::AttachmentStoreOp::eStore;
            attachments[i].initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
            attachments[i].finalLayout = colorFinalLayout;
            vk::AttachmentReference& attachmentReference = colorAttachmentReferences[i];
            attachmentReference.attachment = static_cast<uint32_t>(i);
            attachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
        }

        // Depth attachment
        vk::AttachmentReference depthAttachmentReference;
        {
            vk::AttachmentDescription depthAttachment;
            depthAttachment.format = depthFormat;
            depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout = depthFinalLayout;
            attachments.push_back(depthAttachment);
            depthAttachmentReference.attachment = static_cast<uint32_t>(attachments.size()) - 1;
            depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        }

        std::vector<vk::SubpassDescription> subpasses;
        {
            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            subpass.pDepthStencilAttachment = &depthAttachmentReference;
            subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
            subpass.pColorAttachments = colorAttachmentReferences.data();
            subpasses.push_back(subpass);
        }

        std::vector<vk::SubpassDependency> subpassDependencies;
        {
            vk::SubpassDependency dependency;
            dependency.srcSubpass = 0;
            dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

            dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            switch (colorFinalLayout) {
                case vk::ImageLayout::eShaderReadOnlyOptimal:
                    dependency.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                    break;
                case vk::ImageLayout::eTransferSrcOptimal:
                    dependency.dstAccessMask = vk::AccessFlagBits::eTransferRead;
                    break;
                default:
                    throw std::runtime_error("Unhandled color final layout");
            }
            dependency.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
            subpassDependencies.push_back(dependency);
        }

        if (renderPass) {
            device.destroyRenderPass(renderPass);
        }

        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        renderPassInfo.pSubpasses = subpasses.data();
        //        renderPassInfo.dependencyCount = subpassDependencies.size();
        //        renderPassInfo.pDependencies = subpassDependencies.data();
        renderPass = device.createRenderPass(renderPassInfo);
    }
};
}  // namespace vkx
