#pragma once

#include "context.hpp"
#include "framebuffer.hpp"
#include "ui.hpp"

namespace vkx {
    struct Offscreen {
        const vkx::Context& context;
        vk::RenderPass renderPass;
        vk::Extent2D size;
        std::vector<vk::Format> colorFormats{ vk::Format::eB8G8R8A8Unorm };
        // This value is chosen as an invalid default that signals that the code should pick a specific depth buffer
        // Alternative, you can set this to undefined to explicitly declare you want no depth buffer.
        vk::Format depthFormat{ vk::Format::eR8Uscaled };
        std::vector<vkx::Framebuffer> framebuffers{ MAX_FRAMES_IN_FLIGHT };
        std::vector<vk::DescriptorSet> descriptorSets;
        std::vector<vk::CommandBuffer> commandBuffers;
        vk::ImageUsageFlags attachmentUsage{ vk::ImageUsageFlagBits::eSampled };
        vk::ImageUsageFlags depthAttachmentUsage{ vk::ImageUsageFlagBits::eDepthStencilAttachment };
        vk::ImageLayout colorFinalLayout{ vk::ImageLayout::eShaderReadOnlyOptimal };
        vk::ImageLayout depthFinalLayout{ vk::ImageLayout::eDepthStencilAttachmentOptimal };

        bool active{ false };

        Offscreen(const vkx::Context& context)
                : context{ context } {}

        void create(vkx::UIOverlay& ui) {
            assert(!colorFormats.empty());
            assert(size != vk::Extent2D{});

            if (depthFormat == vk::Format::eR8Uscaled) {
                depthFormat = context.getSupportedDepthFormat();
            }

            if (!renderPass) {
                createRenderPass();
            }

            commandBuffers = context.allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);

            for (auto& framebuffer: framebuffers) {
                framebuffer.create(context, size, colorFormats, depthFormat, renderPass, attachmentUsage, depthAttachmentUsage);
            }

            createSampler();

            assert(framebuffers[0].colors.size() == 1);

            descriptorSets.reserve(MAX_FRAMES_IN_FLIGHT);
            for (auto& framebuffer : framebuffers) {
                auto& image = framebuffer.colors[0];
                descriptorSets.push_back(ui.addTexture(image.sampler, image.view, colorFinalLayout));
            }

            active = true;
        }

        void destroy() {
            if (!active)
                return;

            for (auto& framebuffer: framebuffers) {
                framebuffer.destroy();
            }
            context.device.freeCommandBuffers(context.getCommandPool(), commandBuffers);
            context.device.destroyRenderPass(renderPass);
        }

    protected:
        void createSampler() {
            // Create sampler
            vk::SamplerCreateInfo sampler;
            sampler.magFilter = vk::Filter::eLinear;
            sampler.minFilter = vk::Filter::eLinear;
            sampler.mipmapMode = vk::SamplerMipmapMode::eLinear;
            sampler.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            sampler.addressModeV = vk::SamplerAddressMode::eClampToEdge;
            sampler.addressModeW = vk::SamplerAddressMode::eClampToEdge;
            sampler.mipLodBias = 0.0f;
            sampler.maxAnisotropy = 1.0f;
            sampler.compareOp = vk::CompareOp::eNever;
            sampler.minLod = 0.0f;
            sampler.maxLod = 1.0f;
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
                attachments[i].samples = vk::SampleCountFlagBits::e1;
                attachments[i].loadOp = vk::AttachmentLoadOp::eClear;
                attachments[i].storeOp = colorFinalLayout == vk::ImageLayout::eColorAttachmentOptimal ? vk::AttachmentStoreOp::eDontCare : vk::AttachmentStoreOp::eStore;
                attachments[i].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
                attachments[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
                attachments[i].initialLayout = vk::ImageLayout::eUndefined;
                attachments[i].finalLayout = colorFinalLayout;

                vk::AttachmentReference& attachmentReference = colorAttachmentReferences[i];
                attachmentReference.attachment = i;
                attachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

                subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
                subpass.pColorAttachments = colorAttachmentReferences.data();
            }

            // Do we have a depth format?
            vk::AttachmentReference depthAttachmentReference;
            if (depthFormat != vk::Format::eUndefined) {
                vk::AttachmentDescription depthAttachment;
                depthAttachment.format = depthFormat;
                depthAttachment.samples = vk::SampleCountFlagBits::e1;
                depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
                // We might be using the depth attacment for something, so preserve it if it's final layout is not undefined
                depthAttachment.storeOp = depthFinalLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal
                                          ? vk::AttachmentStoreOp::eDontCare : vk::AttachmentStoreOp::eStore;
                depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
                depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
                depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
                depthAttachment.finalLayout = depthFinalLayout;
                attachments.push_back(depthAttachment);

                depthAttachmentReference.attachment = static_cast<uint32_t>(attachments.size() - 1);
                depthAttachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                subpass.pDepthStencilAttachment = &depthAttachmentReference;
            }

            std::vector<vk::SubpassDependency> subpassDependencies;
            {
                if ((colorFinalLayout != vk::ImageLayout::eColorAttachmentOptimal) &&
                    (colorFinalLayout != vk::ImageLayout::eUndefined)) {
                    // Implicit transition
                    vk::SubpassDependency dependency;
                    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                    dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite; //renderpass A wrote to the image as an attachment.
                    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

                    dependency.dstSubpass = 0;
                    dependency.dstAccessMask = vkx::util::accessFlagsForLayout(colorFinalLayout);  //renderpass B is reading from the image in a shader.
                    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader; //Assuming you're reading the image in the fragment shader. Insert other shader stage(s) if otherwise.

                    //dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;
                    subpassDependencies.push_back(dependency);
                }

                if ((depthFinalLayout != vk::ImageLayout::eDepthStencilAttachmentOptimal) &&
                    (depthFinalLayout != vk::ImageLayout::eUndefined)) {
                    // Implicit transition
                    vk::SubpassDependency dependency;
                    dependency.srcSubpass = 0;
                    dependency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    dependency.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;

                    dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                    dependency.dstAccessMask = vkx::util::accessFlagsForLayout(depthFinalLayout);
                    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

                    //dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;
                    subpassDependencies.push_back(dependency);
                }
            }

            if (renderPass) {
                context.device.destroyRenderPass(renderPass);
            }

            vk::RenderPassCreateInfo renderPassInfo;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
            renderPassInfo.pDependencies = subpassDependencies.data();
            renderPass = context.device.createRenderPass(renderPassInfo);
        }
    };
} // namespace vkx