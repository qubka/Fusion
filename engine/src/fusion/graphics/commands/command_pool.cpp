#include "command_pool.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

CommandPool::CommandPool() {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto graphicsFamily = physicalDevice.getGraphicsFamily();

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = graphicsFamily;
	VK_RESULT(vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool));
}

CommandPool::~CommandPool() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
}
