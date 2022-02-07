#pragma once

#include "Core.hpp"

namespace Fusion {
    class Event;

    class FUSION_API Layer {
    public:
        Layer(std::string name = "Layer") : name{std::move(name)} {}
        virtual ~Layer() = default;

        virtual void onAttach() {};
        virtual void onDetach() {};
        virtual void onUpdate() {};
        virtual void onEvent(Event& event) {};

        const std::string& getName() const { return name; }
    protected:
        std::string name;
    };
}
