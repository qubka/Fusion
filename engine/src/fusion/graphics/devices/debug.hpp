#pragma once

namespace fe {
    class LogicalDevice;
    // Setup and functions for the VK_EXT_debug_marker_extension
    // Extension spec can be found at https://github.com/KhronosGroup/Vulkan-Docs/blob/1.0-VK_EXT_debug_marker/doc/specs/vulkan/appendices/VK_EXT_debug_marker.txt
    // Note that the extension will only be present if run from an offline debugging application
    // The actual check for extension presence and enabling it on the device is done in the example base class
    // See VulkanExampleBase::createInstance and VulkanExampleBase::createDevice (base/vulkanexamplebase.cpp)
    class DebugMarker {
    public:
        DebugMarker();
        ~DebugMarker() = default;
        NONCOPYABLE(DebugMarker);

        // Sets the debug name of an object
        // All Objects in Vulkan are represented by their 64-bit handles which are passed into this function
        // along with the object type
        void setObjectName(uint64_t object, VkDebugReportObjectTypeEXT objectType, std::string_view name);

        // Set the tag for an object
        void setObjectTag(uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);

        // Start a new debug marker region
        void beginRegion(VkCommandBuffer commandBuffer,std::string_view markerName, const glm::vec4& color);

        // Insert a new debug marker into the command buffer
        void insert(VkCommandBuffer commandBuffer, std::string_view markerName, const glm::vec4& color);

        // End the current debug marker region
        void endRegion(VkCommandBuffer commandBuffer);

        // Object specific naming functions
        void setCommandBufferName(VkCommandBuffer commandBuffer, std::string_view name);
        void setQueueName(VkQueue queue, std::string_view name);
        void setImageName(VkImage image, std::string_view name);
        void setSamplerName(VkSampler sampler, std::string_view name);
        void setBufferName(VkBuffer buffer, std::string_view name);
        void setDeviceMemoryName(VkDeviceMemory memory, std::string_view name);
        void setShaderModuleName(VkShaderModule shaderModule, std::string_view name);
        void setPipelineName(VkPipeline pipeline, std::string_view name);
        void setPipelineLayoutName(VkPipelineLayout pipelineLayout, std::string_view name);
        void setRenderPassName(VkRenderPass renderPass, std::string_view name);
        void setFramebufferName(VkFramebuffer framebuffer, std::string_view name);
        void setDescriptorSetLayoutName(VkDescriptorSetLayout descriptorSetLayout, std::string_view name);
        void setDescriptorSetName(VkDescriptorSet descriptorSet, std::string_view name);
        void setSemaphoreName(VkSemaphore semaphore, std::string_view name);
        void setFenceName(VkFence fence, std::string_view name);
        void setEventName(VkEvent event, std::string_view name);

    private:
        const LogicalDevice& logicalDevice;

        PFN_vkDebugMarkerSetObjectTagEXT pfnDebugMarkerSetObjectTag{ VK_NULL_HANDLE };
        PFN_vkDebugMarkerSetObjectNameEXT pfnDebugMarkerSetObjectName{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerBeginEXT pfnCmdDebugMarkerBegin{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerEndEXT pfnCmdDebugMarkerEnd{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerInsertEXT pfnCmdDebugMarkerInsert{ VK_NULL_HANDLE };
    };
}
