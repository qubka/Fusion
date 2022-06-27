#include "fence.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

Fence::Fence() {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    Graphics::CheckVk(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence));
}

Fence::Fence(VkFence fence) : fence{fence} {

}

Fence::~Fence() {
    if (fence != VK_NULL_HANDLE) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        vkDestroyFence(logicalDevice, fence, nullptr);
    }
}