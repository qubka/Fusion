#pragma once

#include <volk/volk.h>

namespace fe {
    class CommandPool;
    class LogicalDevice;
    /**
     * @brief Class that represents a command buffer.
     */
    class CommandBuffer {
    public:
        /**
         * Creates a new command buffer.
         * @param begin If recording will start right away, if true {@link CommandBuffer#Begin} is called.
         * @param queueType The queue to run this command buffer on.
         * @param bufferLevel The buffer level.
         */
        explicit CommandBuffer(bool begin = false, VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        ~CommandBuffer();

        /**
         * Begins the recording state for this command buffer.
         * @param usage How this command buffer will be used.
         */
        void begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        /**
         * Ends the recording state for this command buffer.
         */
        void end();

        /**
         * Submits the command buffer to the queue and will hold the current thread idle until it has finished.
         */
        void submitIdle();

        /**
         * Submits the command buffer.
         * @param waitSemaphore A optional semaphore that will waited upon before the command buffer is executed.
         * @param signalSemaphore A optional that is signaled once the command buffer has been executed.
         * @param fence A optional fence that is signaled once the command buffer has completed.
         * @param submitPipelineStages Bitmask specifying pipeline stage.
         */
        void submit(const VkSemaphore& waitSemaphore = VK_NULL_HANDLE, const VkSemaphore& signalSemaphore = VK_NULL_HANDLE, const VkFence& fence = VK_NULL_HANDLE,
                    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        // Create a short lived command buffer which is immediately executed and released
        // This function is intended for initialization only. It incurs a queue and device
        // flush and may impact performance if used in non-setup code.
        static void SubmitBuffer(const std::function<void(CommandBuffer& commandBuffer)>& function);

        operator bool() const { return commandBuffer != VK_NULL_HANDLE; }
        operator const VkCommandBuffer&() const { return commandBuffer; }

        const VkCommandBuffer& getCommandBuffer() const { return commandBuffer; }
        bool isRunning() const { return running; }

        VkQueue getQueue() const;

    private:
        const LogicalDevice& logicalDevice;

        VkQueueFlagBits queueType;
        VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };
        bool running{ false };
    };
}
