#pragma once

//#include <thread>
#include <volk/volk.h>

namespace fe {
    /**
     * @brief Class that represents a command pool.
     */
    class CommandPool {
    public:
        CommandPool();
        ~CommandPool();

        operator bool() const { return commandPool != VK_NULL_HANDLE; }
        operator const VkCommandPool&() const { return commandPool; }

        const VkCommandPool& getCommandPool() const { return commandPool; }

    private:
        VkCommandPool commandPool{ VK_NULL_HANDLE };
        //std::thread::id threadId;
    };
}
