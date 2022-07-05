#include "subrender_holder.hpp"

using namespace fe;

void SubrenderHolder::clear() {
	stages.clear();
}

void SubrenderHolder::removeSubrenderStage(const std::type_index& id) {
	for (auto it = stages.begin(); it != stages.end();) {
		if (it->second == id) {
			it = stages.erase(it);
		} else {
			++it;
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
