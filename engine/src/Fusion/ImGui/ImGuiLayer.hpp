#pragma once

#include "Fusion/Core/Layer.hpp"

namespace Fusion {
    class FUSION_API ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;

        void onAttach() override;
        void onDetach() override;

        void begin();
        void end();
    private:
        static void setDarkThemeColors();
    };
}
