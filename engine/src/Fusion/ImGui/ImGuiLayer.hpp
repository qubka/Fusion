#pragma once

#include "Fusion/Core/Layer.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Renderer/SwapChain.hpp"

namespace Fusion {
    class FUSION_API ImGuiLayer : public Layer {
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
