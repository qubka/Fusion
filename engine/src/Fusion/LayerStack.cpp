#include "LayerStack.hpp"
#include "Layer.hpp"

using namespace Fusion;

LayerStack::LayerStack() {
    layerInsert = layers.begin();
}

LayerStack::~LayerStack() {
    for (auto* layer : layers) {
        delete layer;
    }
}

void LayerStack::pushLayer(Layer& layer) {
    layerInsert = layers.emplace(layerInsert, &layer);
}

void LayerStack::pushOverlay(Layer& overlay) {
    layers.emplace_back(&overlay);
}

void LayerStack::popLayer(Layer& layer) {
    auto it = std::find(begin(), end(), &layer);
    if (it != end()) {
        layers.erase(it);
        layerInsert--;
    }
}

void LayerStack::popOverlay(Layer& overlay) {
    auto it = std::find(begin(), end(), &overlay);
    if (it != end()) {
        layers.erase(it);
    }
}
