#pragma once

namespace fe {
    class Semaphore {
    public:
        Semaphore();
        Semaphore(VkSemaphore semaphore);
        ~Semaphore();

        operator bool() const { return semaphore != VK_NULL_HANDLE; }
        operator const VkSemaphore&() const { return semaphore; }

        const VkSemaphore& getSemaphore() const { return semaphore; }

    private:
        VkSemaphore semaphore{ VK_NULL_HANDLE };
    };
}
