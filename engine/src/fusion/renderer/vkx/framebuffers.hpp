//
//  Created by Nikita Ushakov on 2022/06/18
//

#pragma once

#include "context.hpp"
#include "swapchain.hpp"

namespace vkx {
    struct Attachment {
        enum class Type { Image, Depth, Swapchain };
        uint32_t binding;
        std::string name;
        Type type;
        bool multisampled;
        vk::Format format;
        vk::ClearColorValue clearColour;
    };

    struct SubpassType {
        uint32_t binding;
        std::vector<uint32_t> attachmentBindings;
    };

    struct Framebuffers {
        vk::Device device;
        std::vector<vk::Framebuffer> framebuffers;
        std::vector<vkx::Image> imageAttachments;

        void create(const vkx::Context& context,
                    const vk::Extent2D& size,
                    const std::vector<vkx::Attachment>& renderAttachments,
                    const vk::RenderPass& renderPass,
                    const vkx::Swapchain& swapchain,
                    const vkx::Image& depthStencil) {
            destroy();
            device = context.device;

            vk::ImageCreateInfo imageCreateInfo;
            imageCreateInfo.imageType = vk::ImageType::e2D;
            imageCreateInfo.extent.width = size.width;
            imageCreateInfo.extent.height = size.height;
            imageCreateInfo.extent.depth = 1;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage;

            imageAttachments.resize(renderAttachments.size());

            for (size_t i = 0; i < renderAttachments.size(); i++) {
                const auto& attachment = renderAttachments[i];
                //auto attachmentSamples = attachment.multisampled ? content.physicalDevice->GetMsaaSamples() : vk::SampleCountFlagBits::e1;

                switch (attachment.type) {
                    case Attachment::Type::Image:
                        imageCreateInfo.format = attachment.format;
                        imageAttachments[i] = context.createImage(imageCreateInfo);
                        break;
                    default:
                        imageAttachments[i] = vkx::Image{};
                        break;
                }
            }

            vk::FramebufferCreateInfo framebufferCreateInfo;
            framebufferCreateInfo.renderPass = renderPass;
            framebufferCreateInfo.width = size.width;
            framebufferCreateInfo.height = size.height;
            framebufferCreateInfo.layers = 1;

            framebuffers.resize(swapchain.imageCount);

            for (size_t i = 0; i < swapchain.imageCount; i++) {
                std::vector<vk::ImageView> attachments;
                attachments.reserve(renderAttachments.size());

                for (const auto& attachment : renderAttachments) {
                    switch (attachment.type) {
                        case Attachment::Type::Image:
                            attachments.emplace_back(imageAttachments[attachment.binding].view);
                            break;
                        case Attachment::Type::Depth:
                            attachments.emplace_back(depthStencil.view);
                            break;
                        case Attachment::Type::Swapchain:
                            attachments.emplace_back(swapchain.images[i].view);
                            break;
                    }
                }

                framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferCreateInfo.pAttachments = attachments.data();
                framebuffers[i] = device.createFramebuffer(framebufferCreateInfo);
            }
        }

        void destroy() {
            for (auto& attachment : imageAttachments) {
                attachment.destroy();
            }
            for (auto& framebuffer : framebuffers) {
                device.destroyFramebuffer(framebuffer);
            }
        }

        size_t size() const { return framebuffers.size(); }
        vk::Framebuffer& operator[](uint32_t index) { return framebuffers[index]; }
        const vk::Framebuffer& operator[](uint32_t index) const { return framebuffers[index]; }
    };
}  // namespace vkx
