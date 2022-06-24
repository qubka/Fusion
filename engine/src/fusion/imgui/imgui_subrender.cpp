#include "imgui_subrender.hpp"

#include "fusion/models/vertex2d.hpp"

using namespace fe;

ImguiSubrender::ImguiSubrender(const Pipeline::Stage& pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"shaders/imgui/gui.vert", "shaders/imgui/gui.frag"}, {}} {
}

void ImguiSubrender::render(const CommandBuffer& commandBuffer) {
    pipeline.bindPipeline(commandBuffer);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    /*for (const auto&screenObject : imgui_objects) {
        if (!screenObject->IsEnabled())
            continue;
        if (auto object = dynamic_cast<ImGui *>(screenObject))
            object->CmdRender(commandBuffer, pipeline);
    }*/
}
