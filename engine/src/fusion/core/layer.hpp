#pragma once

namespace fe {
    class Renderer;
    class Layer {
    public:
        Layer(std::string name = "Layer") : name{std::move(name)} {}
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate(float dt) {}
        virtual void onRender(Renderer& renderer) {}
        virtual void onImGui() {}

        const std::string& getName() const { return name; }

    protected:
        std::string name;
    };
}
