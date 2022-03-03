#pragma once

#include "renderer.hpp"

#include <imgui/imgui.h>

namespace fe {
    class Gui {
        const vkx::Context& context;
        vk::DescriptorPool descriptorPool;

    public:
        Gui(const vkx::Context& context) : context{context} {}
        ~Gui() = default;

        void create(void* window, Renderer& renderer);
        void destroy();

        void begin();
        void end(const vk::CommandBuffer& commandBuffer);

    private:
        static void setDarkThemeColors();
    };
}
