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
#include "texture.hpp"

#include "fusion/core/window.hpp"

#ifdef __ANDROID__
#include <android/native_activity.h>
#endif

namespace vkx { namespace ui {

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
        vkx::Buffer vertexBuffer;
        vkx::Buffer indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        vk::DescriptorPool descriptorPool;
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
        vk::RenderPass renderPass;

        vkx::Image font;

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
        bool header(const char* caption) const;
        bool checkBox(const char* caption, bool* value) const;
        bool checkBox(const char* caption, int32_t* value) const;
        bool inputFloat(const char* caption, float* value, float step, uint32_t precision) const;
        bool sliderFloat(const char* caption, float* value, float min, float max) const;
        bool sliderInt(const char* caption, int32_t* value, int32_t min, int32_t max) const;
        bool comboBox(const char* caption, int32_t* itemindex, const std::vector<std::string>& items) const;
        bool button(const char* caption) const;

        void text(const char* formatstr, ...) const;

        void resize(const vk::Extent2D& size);

        vk::DescriptorSet addTexture(const vk::Sampler& sampler, const vk::ImageView& view, const vk::ImageLayout& layout) const;
    };
}}  // namespace vkx::ui
