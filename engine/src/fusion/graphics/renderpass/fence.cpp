#include "fence.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

Fence::Fence(bool signaled) : signaled{signaled} {
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

bool Fence::wait() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    auto result = vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX);
    VK_CHECK_RESULT(result);
    if (result == VK_SUCCESS) {
        signaled = true;
        return false;
    }

    return true;
}

void Fence::reset() {
    if (signaled) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        VK_CHECK(vkResetFences(logicalDevice, 1, &fence));
    }
    signaled = false;
}

void Fence::waitAndReset() {
    if (!signaled)
        wait();

    reset();
}

bool Fence::checkState() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    auto result = vkGetFenceStatus(logicalDevice, fence);
    if (result == VK_SUCCESS) {
        signaled = true;
        return true;
    }

    return false;
}
