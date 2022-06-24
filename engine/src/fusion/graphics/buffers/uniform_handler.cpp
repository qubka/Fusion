#include "uniform_handler.hpp"

using namespace fe;

UniformHandler::UniformHandler(bool multipipeline)
    : multipipeline{multipipeline}
    , handlerStatus{Buffer::Status::Normal} {
}

UniformHandler::UniformHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline)
    : multipipeline{multipipeline}
    , uniformBlock{uniformBlock}
    , size{static_cast<uint32_t>(this->uniformBlock->getSize())}
    , uniformBuffer{std::make_unique<UniformBuffer>(static_cast<VkDeviceSize>(size))}
    , handlerStatus{Buffer::Status::Normal} {
}

bool UniformHandler::update(const std::optional<Shader::UniformBlock>& uniformBlock) {
	if (handlerStatus == Buffer::Status::Reset || multipipeline && !this->uniformBlock || !multipipeline && this->uniformBlock != uniformBlock) {
		if (size == 0 && !this->uniformBlock || this->uniformBlock && this->uniformBlock != uniformBlock && static_cast<uint32_t>(this->uniformBlock->getSize()) == size) {
			size = static_cast<uint32_t>(uniformBlock->getSize());
		}

		this->uniformBlock = uniformBlock;
		bound = false;
		uniformBuffer = std::make_unique<UniformBuffer>(static_cast<VkDeviceSize>(size));
		handlerStatus = Buffer::Status::Changed;
		return false;
	}

	if (handlerStatus != Buffer::Status::Normal) {
		if (bound) {
			uniformBuffer->unmapMemory();
			bound = false;
		}

		handlerStatus = Buffer::Status::Normal;
	}

	return true;
}
