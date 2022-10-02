#include "descriptors_handler.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/buffers/storage_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

DescriptorsHandler::DescriptorsHandler(const Pipeline& pipeline)
        : shader{&pipeline.getShader()}
        , descriptorSet{std::make_unique<DescriptorSet>(pipeline)}
        , changed{true} {
}

void DescriptorsHandler::push(const std::string& descriptorName, UniformHandler& uniformHandler, const std::optional<OffsetSize>& offsetSize) {
	if (shader) {
		uniformHandler.update(shader->getUniformBlock(descriptorName));
		push(descriptorName, uniformHandler.getUniformBuffer(), offsetSize);
	}
}

void DescriptorsHandler::push(const std::string& descriptorName, StorageHandler& storageHandler, const std::optional<OffsetSize>& offsetSize) {
	if (shader) {
		storageHandler.update(shader->getUniformBlock(descriptorName));
		push(descriptorName, storageHandler.getStorageBuffer(), offsetSize);
	}
}

void DescriptorsHandler::push(const std::string& descriptorName, PushHandler& pushHandler, const std::optional<OffsetSize>& offsetSize) {
	if (shader) {
		pushHandler.update(shader->getUniformBlock(descriptorName));
	}
}

bool DescriptorsHandler::update(const Pipeline& pipeline) {
    auto currentShader = &pipeline.getShader();
	if (shader != currentShader) {
		shader = currentShader;
		descriptors.clear();
		writeDescriptorSets.clear();

		if (!pipeline.isPushDescriptors())
			descriptorSet = std::make_unique<DescriptorSet>(pipeline);

		changed = false;
		return false;
	}

	if (changed) {
		writeDescriptorSets.clear();
		writeDescriptorSets.reserve(descriptors.size());

		for (const auto& [descriptorName, descriptor] : descriptors) {
            auto writeDescriptorSet = descriptor.writeDescriptor.getWriteDescriptorSet();
            if (writeDescriptorSet.descriptorCount == 0)
                continue;

            if (pipeline.isPushDescriptors())
                writeDescriptorSet.dstSet = VK_NULL_HANDLE;
            else
                writeDescriptorSet.dstSet = *descriptorSet;

            writeDescriptorSets.push_back(writeDescriptorSet);
		}

		if (!pipeline.isPushDescriptors())
			descriptorSet->updateDescriptor(writeDescriptorSets);

		changed = false;
	}

	return true;
}

void DescriptorsHandler::bindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline) {
	if (pipeline.isPushDescriptors()) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        Instance::FvkCmdPushDescriptorSetKHR(logicalDevice, commandBuffer, pipeline.getPipelineBindPoint(), pipeline.getPipelineLayout(), 0, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data());
	} else {
        descriptorSet->bindDescriptor(commandBuffer, pipeline);
    }
}
