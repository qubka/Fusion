#include "storage_handler.h"

using namespace fe;

StorageHandler::StorageHandler(bool multipipeline)
        : multipipeline{multipipeline}
        , handlerStatus{Buffer::Status::Reset} {
}

StorageHandler::StorageHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline)
        : multipipeline{multipipeline}
        , uniformBlock{uniformBlock}
        , size{static_cast<uint32_t>(this->uniformBlock->getSize())}
        , storageBuffer{std::make_unique<StorageBuffer>(static_cast<VkDeviceSize>(size))}
        , handlerStatus{Buffer::Status::Changed} {
}

bool StorageHandler::update(const std::optional<Shader::UniformBlock>& uniformBlock) {
	if (handlerStatus == Buffer::Status::Reset || multipipeline && !this->uniformBlock || !multipipeline && this->uniformBlock != uniformBlock) {
		if (size == 0 && !this->uniformBlock || this->uniformBlock && this->uniformBlock != uniformBlock && static_cast<uint32_t>(this->uniformBlock->getSize()) == size) {
			size = static_cast<uint32_t>(uniformBlock->getSize());
		}

		this->uniformBlock = uniformBlock;
		bound = false;
		storageBuffer = std::make_unique<StorageBuffer>(static_cast<VkDeviceSize>(size));
		handlerStatus = Buffer::Status::Changed;
		return false;
	}

	if (handlerStatus != Buffer::Status::Normal) {
		if (bound) {
			storageBuffer->unmap();
			bound = false;
		}

		handlerStatus = Buffer::Status::Normal;
	}

	return true;
}
