#include "model_subrender.hpp"
#include "model.hpp"
#include "mesh.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

ModelSubrender::ModelSubrender(const Pipeline::Stage& pipelineStage) : Subrender{pipelineStage}
        , pipeline{pipelineStage, {"EngineShaders/model/models.vert", "EngineShaders/model/models.frag"}, {{{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::RGBA}}}} {
    cubeModel = std::make_unique<Model>("EngineModels/cube.obj");
}

ModelSubrender::~ModelSubrender() {

}

void ModelSubrender::onRender(const CommandBuffer& commandBuffer) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    auto camera = scene->getCamera();
    if (!camera)
        return;

    // Updates uniform
    pushObject.push("view_projection", camera->getProjectionMatrix() * camera->getViewMatrix());
    pushObject.push("transform", glm::mat4{1});

    // Updates descriptors
    descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    cubeModel->cmdRender(commandBuffer);
}
