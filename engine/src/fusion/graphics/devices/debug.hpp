#pragma once

#include <volk/volk.h>

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
        void setObjectName(uint64_t object, VkDebugReportObjectTypeEXT objectType, const std::string& name);

        // Set the tag for an object
        void setObjectTag(uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);

        // Start a new debug marker region
        void beginRegion(VkCommandBuffer commandBuffer, const std::string& markerName, const glm::vec4& color);

        // Insert a new debug marker into the command buffer
        void insert(VkCommandBuffer commandBuffer, const std::string& markerName, const glm::vec4& color);

        // End the current debug marker region
        void endRegion(VkCommandBuffer commandBuffer);

        // Object specific naming functions
        void setCommandBufferName(VkCommandBuffer commandBuffer, const std::string& name);
        void setQueueName(VkQueue queue, const std::string& name);
        void setImageName(VkImage image, const std::string& name);
        void setSamplerName(VkSampler sampler, const std::string& name);
        void setBufferName(VkBuffer buffer, const std::string& name);
        void setDeviceMemoryName(VkDeviceMemory memory, const std::string& name);
        void setShaderModuleName(VkShaderModule shaderModule, const std::string& name);
        void setPipelineName(VkPipeline pipeline, const std::string& name);
        void setPipelineLayoutName(VkPipelineLayout pipelineLayout, const std::string& name);
        void setRenderPassName(VkRenderPass renderPass, const std::string& name);
        void setFramebufferName(VkFramebuffer framebuffer, const std::string& name);
        void setDescriptorSetLayoutName(VkDescriptorSetLayout descriptorSetLayout, const std::string& name);
        void setDescriptorSetName(VkDescriptorSet descriptorSet, const std::string& name);
        void setSemaphoreName(VkSemaphore semaphore, const std::string& name);
        void setFenceName(VkFence fence, const std::string& name);
        void setEventName(VkEvent event, const std::string& name);

    private:
        const LogicalDevice& logicalDevice;

        PFN_vkDebugMarkerSetObjectTagEXT pfnDebugMarkerSetObjectTag{ VK_NULL_HANDLE };
        PFN_vkDebugMarkerSetObjectNameEXT pfnDebugMarkerSetObjectName{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerBeginEXT pfnCmdDebugMarkerBegin{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerEndEXT pfnCmdDebugMarkerEnd{ VK_NULL_HANDLE };
        PFN_vkCmdDebugMarkerInsertEXT pfnCmdDebugMarkerInsert{ VK_NULL_HANDLE };
    };
}
