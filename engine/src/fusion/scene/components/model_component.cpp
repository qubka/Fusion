#include "model_component.hpp"

#include "fusion/models/model.hpp"

fe::ModelComponent::ModelComponent() {
    model = std::make_shared<Model>("EngineModels/vulkanscenemodels.dae");
}
