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

void SubrenderHolder::renderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
	for (const auto& [stageId, type] : stages) {
		if (stageId != stage) {
			continue;
		}

        if (auto& subrender = subrenders[type.first][type.second]) {
            subrender->onRender(commandBuffer, overrideCamera);
        }
	}
}
