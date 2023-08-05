#pragma once

namespace fe {
    class FUSION_API Fence {
    public:
        Fence(bool signaled = false);
        Fence(VkFence fence);
        ~Fence();

        operator bool() const { return fence != VK_NULL_HANDLE; }
        operator const VkFence&() const { return fence; }

        const VkFence& getFence() const { return fence; }

        bool isSignaled() const { return signaled; };
        void setSignaled(bool flag) { signaled = true; };

        bool wait();
        void reset();
        void waitAndReset();
        bool checkState();

    private:
        VkFence fence{ VK_NULL_HANDLE };
        bool signaled{ false };
    };
}
