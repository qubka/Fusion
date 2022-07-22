#pragma once

#include "fusion/graphics/textures/texture_depth.hpp"
#include "fusion/graphics/renderpass/framebuffers.hpp"
#include "fusion/graphics/renderpass/renderpass.hpp"
#include "fusion/graphics/renderpass/swapchain.hpp"
#include "fusion/graphics/vku.hpp"

namespace fe {
    struct Attachment {
        enum class Type { Image, Depth, Swapchain };
        uint32_t binding{ 0 };
        std::string name;
        Type type{ Type:: Image };
        bool multisampled{ false };
        VkFormat format{ VK_FORMAT_R8G8B8A8_UNORM };
        glm::vec4 clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    };

    struct SubpassType {
        uint32_t binding{ 0 };
        std::vector<uint32_t> attachmentBindings{};
    };

    struct RenderArea {
        glm::uvec2 extent{ 0 };
        glm::ivec2 offset{ 0 };

        bool operator==(const RenderArea& rhs) const {
            return extent == rhs.extent && offset == rhs.offset;
        }
        bool operator!=(const RenderArea& rhs) const {
            return !operator==(rhs);
        }
    };

    struct Viewport {
        glm::vec2 scale{ 1.0f };
        std::optional<glm::uvec2> size;
        glm::ivec2 offset{ 0 };

        bool operator==(const Viewport& rhs) const {
            return scale == rhs.scale && offset == rhs.offset && size == rhs.size;
        }
        bool operator!=(const Viewport& rhs) const {
            return !operator==(rhs);
        }
    };

    class RenderStage {
        friend class Graphics;
    public:
        explicit RenderStage(std::vector<Attachment> images = {}, std::vector<SubpassType> subpasses = {}, const Viewport& viewport = {});
        ~RenderStage() = default;
        NONCOPYABLE(RenderStage);

        void update(size_t id, const Swapchain& swapchain);
        void rebuild(size_t id, const Swapchain& swapchain);

        std::optional<Attachment> getAttachment(const std::string& name) const;
        std::optional<Attachment> getAttachment(uint32_t binding) const;

        const Descriptor* getDescriptor(const std::string& name) const;
        const VkFramebuffer& getActiveFramebuffer(uint32_t activeSwapchainImage) const;

        const std::vector<Attachment>& getAttachments() const { return attachments; }
        const std::vector<SubpassType>& getSubpasses() const { return subpasses; }

        Viewport& getViewport() { return viewport; }
        bool setViewport(const Viewport& port) const;

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

        const Renderpass* getRenderpass() const { return renderpass.get(); }
        const TextureDepth* getDepthStencil() const { return depthStencil.get(); }
        const Framebuffers* getFramebuffers() const { return framebuffers.get(); }
        const std::vector<VkClearValue>& getClearValues() const { return clearValues; }
        uint32_t getAttachmentCount(uint32_t subpass) const { return subpassAttachmentCount[subpass]; }
        bool hasDepth() const { return depthAttachment.has_value(); }
        bool hasSwapchain() const { return swapchainAttachment.has_value(); }
        bool isMultisampled(uint32_t subpass) const { return subpassMultisampled[subpass]; }

    private:
        std::vector<Attachment> attachments;
        std::vector<SubpassType> subpasses;

        mutable Viewport viewport;

        std::unique_ptr<Renderpass> renderpass;
        std::unique_ptr<TextureDepth> depthStencil;
        std::unique_ptr<Framebuffers> framebuffers;

        std::unordered_map<std::string, const Descriptor*> descriptors;

        std::vector<VkClearValue> clearValues;
        std::vector<uint32_t> subpassAttachmentCount;
        std::optional<Attachment> depthAttachment;
        std::optional<Attachment> swapchainAttachment;
        std::vector<bool> subpassMultisampled;

        RenderArea renderArea;
        bool outOfDate{ false };
    };
}
