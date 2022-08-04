#include "subrender_holder.hpp"

using namespace fe;

void SubrenderHolder::clear() {
	stages.clear();
    subrenders.clear();
}

void SubrenderHolder::updateAll() {
    for (auto& [type, subrender] : subrenders) {
        for (auto& i : subrender) {
            if (i->isEnabled()) {
                i->onUpdate();
            }
        }
    }
}

void SubrenderHolder::renderStage(Pipeline::Stage pipelineStage, const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
	for (const auto& [stageId, type] : stages) {
		if (stageId != pipelineStage) {
			continue;
		}

        if (auto& subrender = subrenders[type.first][type.second]) {
            if (subrender->isEnabled()) {
                subrender->onRender(commandBuffer, overrideCamera);
            }
        }
	}
}
