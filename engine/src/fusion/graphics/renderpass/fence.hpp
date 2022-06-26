#pragma once

#include "fusion/graphics/graphics.hpp"

#include <volk.h>

namespace fe {
    class Fence {
    public:
        Fence() {
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
            Graphics::CheckVk(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence));
        }
        Fence(VkFence fence) : fence{fence} {}
        ~Fence() {
            if (fence != VK_NULL_HANDLE) {
                const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
                vkDestroyFence(logicalDevice, fence, nullptr);
            }
        }

        operator bool() const { return fence != VK_NULL_HANDLE; }
        operator const VkFence&() const { return fence; }
        const VkFence& getFence() const { return fence; }

    private:
        VkFence fence{ VK_NULL_HANDLE };
    };
}
