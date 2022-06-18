#pragma once

#include "fusion/renderer/vkx/context.hpp"
#include "fusion/renderer/vkx/helpers.hpp"
#include "fusion/renderer/vkx/image.hpp"

namespace fe {
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

    struct RenderArea {
        glm::uvec2 extent;
        glm::ivec2 offset;
        float aspectRatio{ 1.0f };
    };

    struct Viewport {
        glm::vec2 scale{ 1.0f };
        std::optional<glm::uvec2> size;
        glm::ivec2 offset;
    };

    class RenderStage {
        friend class Graphics;
    public:
        RenderStage(std::vector<Attachment> images = {}, std::vector<SubpassType> subpasses = {}, const Viewport& viewport = Viewport{});
        ~RenderStage();

        void update();
        void rebuild(const Swapchain& swapchain);

        std::optional<Attachment> getAttachment(const std::string& name) const;
        std::optional<Attachment> getAttachment(uint32_t binding) const;

        const Descriptor* getDescriptor(const std::string& name) const;
        const vk::Framebuffer& getActiveFramebuffer(uint32_t activeSwapchainImage) const;

        const std::vector<Attachment>& getAttachments() const { return attachments; }
        const std::vector<SubpassType>& getSubpasses() const { return subpasses; }

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
        const Renderpass& getRenderpass() const { return renderpass.get(); }
        const vkx::Image& getDepthStencil() const { return depthStencil; }
        const Framebuffers* getFramebuffers() const { return framebuffers.get(); }
        const std::vector<vk::ClearValue>& getClearValues() const { return clearValues; }
        uint32_th GetAttachmentCount(uint32_t subpass) const { return subpassAttachmentCount[subpass]; }
        bool hasDepth() const { return depthAttachment.has_value(); }
        bool hasSwapchain() const { return swapchainAttachment.has_value(); }
        bool isMultisampled(uint32_t subpass) const { return subpassMultisampled[subpass]; }

        private:
        std::vector<Attachment> attachments;
        std::vector<SubpassType> subpasses;

        Viewport viewport;

        std::unique_ptr<vk::RenderPass> renderpass;
        vkx::Image depthStencil;
        vkx::Image framebuffers;

        std::map<std::string, const Descriptor *> descriptors;

        std::vector<vk::ClearValue> clearValues;
        std::vector<uint32_t> subpassAttachmentCount;
        std::optional<Attachment> depthAttachment;
        std::optional<Attachment> swapchainAttachment;
        std::vector<bool> subpassMultisampled;

        RenderArea renderArea;
        bool outOfDate{ false };
    };
}
