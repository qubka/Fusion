#include "layer_stack.hpp"
#include "layer.hpp"

using namespace fe;

LayerStack::~LayerStack() {
    for (auto* layer : layers) {
        layer->onDetach();
        delete layer;
    }
}

void LayerStack::pushFront(Layer* layer) {
    layers.push_front(layer);
    layer->onAttach();
}

void LayerStack::pushBack(Layer* layer) {
    layers.push_back(layer);
    layer->onAttach();
}

Layer* LayerStack::pollFront() {
    return layers.empty() ? nullptr : removeFront();
}

Layer* LayerStack::pollBack() {
    return layers.empty() ? nullptr : removeBack();
}

Layer* LayerStack::removeFront() {
    auto* layer = layers.front();
    layer->onDetach();
    layers.pop_front();
    return layer;
}

Layer* LayerStack::removeBack() {
    auto* layer = layers.back();
    layer->onDetach();
    layers.pop_front();
    return layer;
}

void LayerStack::remove(Layer* layer) {
    auto it { std::find(layers.begin(), layers.end(), layer) };
    if (it != layers.end()) {
        layer->onDetach();
        layers.erase(it);
    }
}
