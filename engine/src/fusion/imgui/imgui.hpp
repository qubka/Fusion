#pragma once

#include "fusion/graphics/pipelines/shader.hpp"

#include <imgui/imgui.h>

namespace fe {
    class ImGuis {;
    public:
        static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0) {
            std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
                    {baseBinding, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}
            };
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
                    {0, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)},
                    {1, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)},
                    {2, baseBinding, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}
            };
            return {bindingDescriptions, attributeDescriptions};
        }

        static void SetStyleColors();
        static void UpdateKeyModifiers(int mods);
        static int TranslateUntranslatedKey(int key, int scancode);
        static ImGuiKey KeyToImGuiKey(int key);
    };
}
