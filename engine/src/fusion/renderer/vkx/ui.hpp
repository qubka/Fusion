/*
* UI overlay class using ImGui
*
* Copyright (C) 2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "context.hpp"
#include "framebuffer.hpp"
#include "font.hpp"

#include "fusion/renderer/window.hpp"

#ifdef __ANDROID__
#include <android/native_activity.h>
#endif

namespace vkx {
    struct UIOverlayCreateInfo {
        vk::RenderPass renderPass;
        vk::Extent2D size;
        std::vector<vk::PipelineShaderStageCreateInfo> shaders;
        vk::Format colorFormat{ vk::Format::eB8G8R8A8Unorm };
        vk::Format depthFormat{ vk::Format::eUndefined };
        vk::SampleCountFlagBits rasterizationSamples{ vk::SampleCountFlagBits::e1 };
        uint32_t subpassCount{ 1 };
        uint32_t attachmentCount{ 1 };
        fe::Window* window{ nullptr };
    };

    class UIOverlay {
    private:
        UIOverlayCreateInfo createInfo;
        const vkx::Context& context;
        const vk::Device& device{ context.device };

        vkx::Buffer vertexBuffer;
        vkx::Buffer indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        vk::DescriptorPool descriptorPool;
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
        vk::RenderPass renderPass;

        vkx::Font font;

        fe::Window* currentWindow{ nullptr };
        glm::vec2 lastValidMousePos{ -FLT_MAX };

        struct PushConstBlock {
            glm::vec2 scale;
            glm::vec2 translate;
        } pushConstBlock;

        void prepareResources();
        void preparePipeline();
        void prepareRenderPass();
        void prepareEvents();

        static void setStyleColors();

    public:
        bool active{ false };
        float scale{ 1.0f };

        UIOverlay(const vkx::Context& context) : context{context} {}
        ~UIOverlay();

        void create(const UIOverlayCreateInfo& createInfo);
        void destroy();
        bool update();

        void draw(const vk::CommandBuffer& commandBuffer);
        void resize(const vk::Extent2D& size);

        [[nodiscard]] vk::DescriptorSet addTexture(const vk::Sampler& sampler, const vk::ImageView& view, const vk::ImageLayout& layout) const;

        void onMouseButtonEvent(fe::MouseData data);
        void onMouseMotionEvent(const glm::vec2& pos);
        void onMouseScrollEvent(const glm::vec2& offset);
        void onMouseEnterEvent(bool entered);
        void onKeyEvent(fe::KeyData data);
        void onCharInputEvent(uint32_t c);
        void onFocusEvent(bool focuses);
    };
}  // namespace vkx
