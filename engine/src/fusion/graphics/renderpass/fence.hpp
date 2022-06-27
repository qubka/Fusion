#pragma once

#include <volk.h>

namespace fe {
    class Fence {
    public:
        Fence();
        Fence(VkFence fence);
        ~Fence();

        operator bool() const { return fence != VK_NULL_HANDLE; }
        operator const VkFence&() const { return fence; }

        const VkFence& getFence() const { return fence; }

    private:
        VkFence fence{ VK_NULL_HANDLE };
    };
}
