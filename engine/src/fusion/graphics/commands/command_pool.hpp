#pragma once

#include <thread>
#include <volk.h>

namespace fe {
    /**
     * @brief Class that represents a command pool.
     */
    class CommandPool {
    public:
        explicit CommandPool(const std::thread::id& threadId = std::this_thread::get_id());
        ~CommandPool();

        operator bool() const { return commandPool != VK_NULL_HANDLE; }
        operator const VkCommandPool&() const { return commandPool; }

        const VkCommandPool& getCommandPool() const { return commandPool; }
        const std::thread::id& getThreadId() const { return threadId; }

    private:
        VkCommandPool commandPool{ VK_NULL_HANDLE };
        std::thread::id threadId;
    };
}
