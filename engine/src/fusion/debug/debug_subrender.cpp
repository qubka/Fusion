#include "debug_subrender.h"

#include "fusion/graphics/cameras/camera.h"
#include "fusion/scene/scene_manager.h"

using namespace fe;

DebugSubrender::DebugSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipelines{
    PipelineGraphics{pipelineStage,
                     {FUSION_ASSET_PATH "shaders/debug/line.vert", FUSION_ASSET_PATH "shaders/debug/line.frag"},
                     {{{Vertex::Component::Position, Vertex::Component::RGBA}}},
                     {},
                     PipelineGraphics::Mode::Polygon,
                     PipelineGraphics::Depth::ReadWrite,
                     VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                     VK_POLYGON_MODE_FILL},
    PipelineGraphics{pipelineStage,
                     {FUSION_ASSET_PATH "shaders/debug/line.vert", FUSION_ASSET_PATH "shaders/debug/line.frag"},
                     {{{Vertex::Component::Position, Vertex::Component::RGBA}}},
                     {},
                     PipelineGraphics::Mode::Polygon,
                     PipelineGraphics::Depth::None,
                     VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                     VK_POLYGON_MODE_FILL},
    PipelineGraphics{pipelineStage,
                     {FUSION_ASSET_PATH "shaders/debug/point.vert", FUSION_ASSET_PATH "shaders/debug/point.frag"},
                     {{{Vertex::Component::Position, Vertex::Component::RGBA, Vertex::Component::DummyFloat}}},
                     {},
                     PipelineGraphics::Mode::Polygon,
                     PipelineGraphics::Depth::ReadWrite,
                     VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
                     VK_POLYGON_MODE_FILL},
    PipelineGraphics{pipelineStage,
                     {FUSION_ASSET_PATH "shaders/debug/point.vert", FUSION_ASSET_PATH "shaders/debug/point.frag"},
                     {{{Vertex::Component::Position, Vertex::Component::RGBA, Vertex::Component::DummyFloat}}},
                     {},
                     PipelineGraphics::Mode::Polygon,
                     PipelineGraphics::Depth::None,
                     VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
                     VK_POLYGON_MODE_FILL}}
    , pushObject{true} {
}

DebugSubrender::~DebugSubrender() {

}

void DebugSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniform
    pushObject.push("projection", camera->getProjectionMatrix());
    pushObject.push("view", camera->getViewMatrix());
    pushObject.update(pipelines[0].getShader().getUniformBlock("PushObject"));

    for (int i = 0; i < 2; ++i) {
        bool depthTested = (i == 0);

        // Bind pipeline
        pipelines[i].bindPipeline(commandBuffer);
        pushObject.bindPush(commandBuffer, pipelines[i]);

        // Draw objects
        lines[i].cmdRender(commandBuffer, DebugRenderer::Get()->getLines(depthTested), 1.0f);
        thickLines[i].cmdRender(commandBuffer, DebugRenderer::Get()->getThickLines(depthTested), 2.0f);
        triangles[i].cmdRender(commandBuffer, DebugRenderer::Get()->getTriangles(depthTested), 1.0f);
    }

    for (int i = 0; i < 2; ++i) {
        bool depthTested = (i == 0);

        // Bind pipeline
        pipelines[i + 2].bindPipeline(commandBuffer);
        pushObject.bindPush(commandBuffer, pipelines[i + 2]);

        // Draw objects
        points[i].cmdRender(commandBuffer, DebugRenderer::Get()->getPoints(depthTested));
    }
}