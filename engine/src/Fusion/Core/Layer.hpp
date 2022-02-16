#pragma once

namespace Fusion {
    class FUSION_API Layer {
    public:
        Layer(std::string name = "Layer") : name{std::move(name)} {}
        virtual ~Layer() = default;
        FE_NONCOPYABLE(Layer);

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate() {}
        virtual void onRender() {}
        virtual void onImGui() {}

        const std::string& getName() const { return name; }
    protected:
        std::string name;
    };
}
