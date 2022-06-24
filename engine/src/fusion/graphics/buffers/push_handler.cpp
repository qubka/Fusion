#include "push_handler.hpp"

using namespace fe;

PushHandler::PushHandler(bool multipipeline) : multipipeline{multipipeline} {
}

PushHandler::PushHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline)
    : multipipeline{multipipeline}
    , uniformBlock{uniformBlock}
    , data{std::make_unique<char[]>(this->uniformBlock->getSize())} {
}

bool PushHandler::update(const std::optional<Shader::UniformBlock>& uniformBlock) {
	if ((multipipeline && !this->uniformBlock) || (!multipipeline && this->uniformBlock != uniformBlock)) {
		this->uniformBlock = uniformBlock;
		data = std::make_unique<char[]>(this->uniformBlock->getSize());
		return false;
	}

	return true;
}

void PushHandler::bindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline) {
	vkCmdPushConstants(commandBuffer, pipeline.getPipelineLayout(), uniformBlock->getStageFlags(), 0, static_cast<uint32_t>(uniformBlock->getSize()), data.get());
}