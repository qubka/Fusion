#include "command_pool.h"

#include "fusion/graphics/graphics.h"

using namespace fe;

CommandPool::CommandPool(const std::thread::id& threadId) : threadId{threadId} {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto graphicsFamily = physicalDevice.getGraphicsFamily();

	VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = graphicsFamily;
	VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool));
}

CommandPool::~CommandPool() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
}
