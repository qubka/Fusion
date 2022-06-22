#include "command_pool.hpp"

#include "fusion/renderer/graphics.hpp"

using namespace fe;

CommandPool::CommandPool(const std::thread::id &threadId) : threadId{threadId} {
	auto logicalDevice = Graphics::Get()->getLogicalDevice();
	auto graphicsFamily = logicalDevice->getGraphicsFamily();

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = graphicsFamily;
	Graphics::CheckVk(vkCreateCommandPool(*logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool));
}

CommandPool::~CommandPool() {
	auto logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyCommandPool(*logicalDevice, commandPool, nullptr);
}
