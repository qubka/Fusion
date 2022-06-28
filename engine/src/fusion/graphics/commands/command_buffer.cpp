#include "command_buffer.hpp"
#include "command_pool.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

CommandBuffer::CommandBuffer(bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
    : commandPool{Graphics::Get()->getCommandPool()}
    , queueType{queueType} {
	const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = *commandPool;
	commandBufferAllocateInfo.level = bufferLevel;
	commandBufferAllocateInfo.commandBufferCount = 1;
	Graphics::CheckVk(vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, &commandBuffer));

	if (begin)
        CommandBuffer::begin();
}

CommandBuffer::~CommandBuffer() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    //vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	vkFreeCommandBuffers(logicalDevice, commandPool->getCommandPool(), 1, &commandBuffer);
}

void CommandBuffer::begin(VkCommandBufferUsageFlags usage) {
	if (running)
		return;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = usage;
	Graphics::CheckVk(vkBeginCommandBuffer(commandBuffer, &beginInfo));
	running = true;
}

void CommandBuffer::end() {
	if (!running)
        return;

	Graphics::CheckVk(vkEndCommandBuffer(commandBuffer));
	running = false;
}

void CommandBuffer::submitIdle() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto queueSelected = getQueue();

	if (running)
		end();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;
	Graphics::CheckVk(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence));

	//Graphics::CheckVk(vkResetFences(logicalDevice, 1, &fence));
	Graphics::CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
	Graphics::CheckVk(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));

	vkDestroyFence(logicalDevice, fence, nullptr);
}

void CommandBuffer::submit(const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence fence, VkPipelineStageFlags submitPipelineStages) {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto queueSelected = getQueue();

	if (running)
		end();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (waitSemaphore != VK_NULL_HANDLE) {
		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore;
	} else {
        LOG_DEBUG << "No wait semaphore!";
    }

	if (signalSemaphore != VK_NULL_HANDLE) {
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;
	} else {
        LOG_DEBUG << "No signal semaphore!";
    }

	if (fence != VK_NULL_HANDLE) {
        //Graphics::CheckVk(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));
        Graphics::CheckVk(vkResetFences(logicalDevice, 1, &fence));
    } else {
        LOG_DEBUG << "No flight fence!";
    }

	Graphics::CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
}

VkQueue CommandBuffer::getQueue() const {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	switch (queueType) {
        case VK_QUEUE_GRAPHICS_BIT:
            return logicalDevice.getGraphicsQueue();
        case VK_QUEUE_COMPUTE_BIT:
            return logicalDevice.getComputeQueue();
        default:
            return nullptr;
	}
}
