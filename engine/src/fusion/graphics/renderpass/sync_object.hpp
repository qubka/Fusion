#pragma once

namespace fe {
    class SyncObject {
    public:
        SyncObject();
        ~SyncObject();

        const VkSemaphore& getImageAvailableSemaphore() const { return imageAvailableSemaphore; }
        const VkSemaphore& getRenderFinishedSemaphore() const { return renderFinishedSemaphore; }
        const VkFence& getInFlightFence() const { return inFlightFence; }
        const VkFence& getImageInFlightFence() const { return imageInFlight; }
        void setImageInFlight(VkFence fence) { imageInFlight = fence; }
        void setImageInUse() { imageInFlight = inFlightFence; }

        void reset();

    private:
        void init(VkDevice device);
        void destroy(VkDevice device);

        VkSemaphore imageAvailableSemaphore{ VK_NULL_HANDLE };
        VkSemaphore renderFinishedSemaphore{ VK_NULL_HANDLE };
        VkFence inFlightFence{ VK_NULL_HANDLE };
        VkFence imageInFlight{ VK_NULL_HANDLE };
    };
}
