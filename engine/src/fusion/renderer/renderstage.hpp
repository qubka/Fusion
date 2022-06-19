#pragma once

#include "fusion/renderer/vkx/context.hpp"
#include "fusion/renderer/vkx/helpers.hpp"
#include "fusion/renderer/vkx/image.hpp"
#include "fusion/renderer/vkx/swapchain.hpp"
#include "fusion/renderer/vkx/framebuffers.hpp"

namespace fe {
    struct RenderArea {
        glm::uvec2 extent;
        glm::ivec2 offset;
        float aspectRatio{ 1.0f };

        bool operator==(const RenderArea &rhs) const {
            return extent == rhs.extent && offset == rhs.offset;
        }

        bool operator!=(const RenderArea &rhs) const {
            return !operator==(rhs);
        }
    };

    struct Viewport {
        glm::vec2 scale{ 1.0f };
        std::optional<glm::uvec2> size;
        glm::ivec2 offset;
    };


    class RenderStage {
        friend class Graphics;
    public:
        RenderStage(std::vector<vkx::Attachment> images = {}, std::vector<vkx::SubpassType> subpasses = {}, const Viewport& viewport = {});
        ~RenderStage() = default;
        FE_NONCOPYABLE(RenderStage);

        void update();
        void rebuild(const vkx::Swapchain& swapchain);

        std::optional<vkx::Attachment> getAttachment(const std::string& name) const;
        std::optional<vkx::Attachment> getAttachment(uint32_t binding) const;

        //const Descriptor* getDescriptor(const std::string& name) const;
        const vk::Framebuffer& getActiveFramebuffer(uint32_t activeSwapchainImage) const;

        const std::vector<vkx::Attachment>& getAttachments() const { return attachments; }
        const std::vector<vkx::SubpassType>& getSubpasses() const { return subpasses; }

        Viewport& getViewport() { return viewport; }
        void setViewport(const Viewport& port) { viewport = port; }

        /**
         * Gets the render stage viewport.
         * @return The the render stage viewport.
         */
        const RenderArea& getRenderArea() const { return renderArea; }

        /**
         * Gets if the width or height has changed between the last update and now.
         * @return If the width or height has changed.
         */
        bool isOutOfDate() const { return outOfDate; }
        const vk::RenderPass* getRenderpass() const { return renderpass.get(); }
        const vkx::Image& getDepthStencil() const { return depthStencil; }
        const vkx::Framebuffers& getFramebuffers() const { return framebuffers; }
        const std::vector<vk::ClearValue>& getClearValues() const { return clearValues; }
        uint32_t getAttachmentCount(uint32_t subpass) const { return subpassAttachmentCount[subpass]; }
        bool hasDepth() const { return depthAttachment.has_value(); }
        bool hasSwapchain() const { return swapchainAttachment.has_value(); }
        bool isMultisampled(uint32_t subpass) const { return subpassMultisampled[subpass]; }

    private:
        std::vector<vkx::Attachment> attachments;
        std::vector<vkx::SubpassType> subpasses;

        std::unique_ptr<vk::RenderPass> renderpass;
        vkx::Image depthStencil;
        vkx::Framebuffers framebuffers;

        //std::map<std::string, const Descriptor*> descriptors;

        std::vector<vk::ClearValue> clearValues;
        std::vector<uint32_t> subpassAttachmentCount;
        std::optional<vkx::Attachment> depthAttachment;
        std::optional<vkx::Attachment> swapchainAttachment;
        std::vector<bool> subpassMultisampled;

        Viewport viewport;
        RenderArea renderArea;
        bool outOfDate{ false };
    };
}
