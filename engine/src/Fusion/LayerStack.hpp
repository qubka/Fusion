#pragma once

#include "Core.hpp"

namespace Fusion {
    class Layer;

    class FUSION_API LayerStack {
    public:
        LayerStack();
        ~LayerStack();
        LayerStack(const LayerStack&) = delete;
        LayerStack(LayerStack&&) = delete;
        LayerStack& operator=(const LayerStack&) = delete;
        LayerStack& operator=(LayerStack&&) = delete;

        void pushLayer(Layer& layer);
        void pushOverlay(Layer& overlay);
        void popLayer(Layer& layer);
        void popOverlay(Layer& overlay);

        std::vector<Layer*>::iterator begin() { return layers.begin(); }
        std::vector<Layer*>::iterator end() { return layers.end(); }
    private:
        std::vector<Layer*> layers;
        std::vector<Layer*>::iterator layerInsert;
    };
}
