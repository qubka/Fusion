#include "command_buffer.hpp"
#include "command_pool.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

CommandBuffer::CommandBuffer(bool begin, VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel)
        : logicalDevice{Graphics::Get()->getLogicalDevice()}
        , commandPool{Graphics::Get()->getCommandPool()}
        , queueType{queueType} {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	commandBufferAllocateInfo.commandPool = *commandPool;
	commandBufferAllocateInfo.level = bufferLevel;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VK_CHECK(vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, &commandBuffer));

	if (begin)
        CommandBuffer::begin();
}

CommandBuffer::~CommandBuffer() {
	vkFreeCommandBuffers(logicalDevice, *commandPool, 1, &commandBuffer);
}

void CommandBuffer::begin(VkCommandBufferUsageFlags usage) {
	if (running)
		return;

	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = usage;
	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
	running = true;
}

void CommandBuffer::end() {
	if (!running)
        return;

    VK_CHECK(vkEndCommandBuffer(commandBuffer));
	running = false;
}

void CommandBuffer::submitIdle() {
	auto queueSelected = getQueue();

	if (running)
		end();

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

	VkFence fence;
	VK_CHECK(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence));

	//VK_RESULT(vkResetFences(logicalDevice, 1, &fence));
	VK_CHECK(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
	VK_CHECK(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));

	vkDestroyFence(logicalDevice, fence, nullptr);
}

void CommandBuffer::submit(VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkPipelineStageFlags submitPipelineStages) {
	auto queueSelected = getQueue();

	if (running)
		end();

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (waitSemaphore != VK_NULL_HANDLE) {
		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore;
	}

	if (signalSemaphore != VK_NULL_HANDLE) {
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;
	}

	if (fence != VK_NULL_HANDLE) {
        //VK_RESULT(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(logicalDevice, 1, &fence));
    }

	VK_CHECK(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
}

VkQueue CommandBuffer::getQueue() const {
	switch (queueType) {
        case VK_QUEUE_GRAPHICS_BIT:
            return logicalDevice.getGraphicsQueue();
        case VK_QUEUE_COMPUTE_BIT:
            return logicalDevice.getComputeQueue();
        default:
            return nullptr;
	}
}

void CommandBuffer::SubmitBuffer(const std::function<void(CommandBuffer&)>& function) {
    CommandBuffer commandBuffer{true};
    function(commandBuffer);
    commandBuffer.submitIdle();
}
