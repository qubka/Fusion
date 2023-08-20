#include "push_handler.h"

#include "fusion/graphics/pipelines/pipeline.h"
#include "fusion/graphics/commands/command_buffer.h"

using namespace fe;

PushHandler::PushHandler(bool multipipeline) : multipipeline{multipipeline} {
}

PushHandler::PushHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline)
        : multipipeline{multipipeline}
        , uniformBlock{uniformBlock}
        , data{std::make_unique<uint8_t[]>(this->uniformBlock->getSize())} {
}

bool PushHandler::update(const std::optional<Shader::UniformBlock>& uniformBlock) {
	if ((multipipeline && !this->uniformBlock) || (!multipipeline && this->uniformBlock != uniformBlock)) {
		this->uniformBlock = uniformBlock;
		data = std::make_unique<uint8_t[]>(this->uniformBlock->getSize());
		return false;
	}

	return true;
}

void PushHandler::bindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline) {
	vkCmdPushConstants(commandBuffer, pipeline.getPipelineLayout(), uniformBlock->getStageFlags(), 0, static_cast<uint32_t>(uniformBlock->getSize()), data.get());
}