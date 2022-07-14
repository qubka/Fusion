#include "subrender_holder.hpp"

using namespace fe;

void SubrenderHolder::clear() {
	stages.clear();
    subrenders.clear();
}

void SubrenderHolder::updateAll() {
    for (auto& [typeId, subrender] : subrenders) {
        if (subrender->isEnabled()) {
            subrender->onUpdate();
        }
    }
}

void SubrenderHolder::renderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer) {
	for (const auto& [stageIndex, typeId] : stages) {
		if (stageIndex.first != stage) {
			continue;
		}

		if (auto& subrender = subrenders[typeId]) {
			if (subrender->isEnabled()) {
				subrender->onRender(commandBuffer);
			}
		}
	}
}
