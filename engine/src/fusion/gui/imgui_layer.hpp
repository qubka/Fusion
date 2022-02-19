#pragma once

#include "fusion/core/layer.hpp"
/*#include "fusion/renderer/renderer.hpp"
#include "fusion/renderer/swap_chain.hpp"

namespace Fusion {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(Vulkan& vulkan, Renderer& renderer);
        ~ImGuiLayer() override;

        void onAttach() override;
        void onDetach() override;

        void begin();
        void end(vk::CommandBuffer& commandBuffer);

    private:
        Vulkan& vulkan;
        Renderer& renderer;

        vk::DescriptorPool imguiPool;
        //std::vector<ImTextureID> viewportImages;

        static void setDarkThemeColors();
    };
}
*/