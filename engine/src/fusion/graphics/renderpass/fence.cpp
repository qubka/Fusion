#include "fence.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

Fence::Fence(bool signaled) {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled)
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    VK_CHECK(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence));
}

Fence::Fence(VkFence fence) : fence{fence} {
}

Fence::~Fence() {
    if (fence != VK_NULL_HANDLE) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        vkDestroyFence(logicalDevice, fence, nullptr);
    }
}