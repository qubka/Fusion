#include "subrender_holder.hpp"

using namespace fe;

void SubrenderHolder::clear() {
	stages.clear();
    subrenders.clear();
}

void SubrenderHolder::updateAll() {
    for (auto& [typ, subrender] : subrenders) {
        if (subrender->isEnabled()) {
            subrender->onUpdate();
        }
    }
}

void SubrenderHolder::renderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
	for (const auto& [stageId, type] : stages) {
		if (stageId != stage) {
			continue;
		}

		if (auto& subrender = subrenders[type]) {
			if (subrender->isEnabled()) {
				subrender->onRender(commandBuffer, overrideCamera);
			}
		}
	}
}
