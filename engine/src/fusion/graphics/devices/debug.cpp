#include "debug.hpp"
#include "logical_device.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

DebugMarker::DebugMarker() : logicalDevice{Graphics::Get()->getLogicalDevice()} {
    pfnDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectTagEXT"));
    if (!pfnDebugMarkerSetObjectTag) {
        LOG_ERROR << "vkDebugMarkerSetObjectTagEXT not exist!";
    }
    pfnDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectNameEXT"));
    if (!pfnDebugMarkerSetObjectName) {
        LOG_ERROR << "vkDebugMarkerSetObjectNameEXT not exist!";
    }
    pfnCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerBeginEXT"));
    if (!pfnCmdDebugMarkerBegin) {
        LOG_ERROR << "vkCmdDebugMarkerBeginEXT not exist!";
    }
    pfnCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerEndEXT"));
    if (!pfnCmdDebugMarkerEnd) {
        LOG_ERROR << "vkCmdDebugMarkerEndEXT not exist!";
    }
    pfnCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerInsertEXT"));
    if (!pfnCmdDebugMarkerInsert) {
        LOG_ERROR << "vkCmdDebugMarkerInsertEXT not exist!";
    }
}

void DebugMarker::setObjectName(uint64_t object, VkDebugReportObjectTypeEXT objectType, const std::string& name) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (pfnDebugMarkerSetObjectName) {
        VkDebugMarkerObjectNameInfoEXT nameInfo = {};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = objectType;
        nameInfo.object = object;
        nameInfo.pObjectName = name.data();
        pfnDebugMarkerSetObjectName(logicalDevice, &nameInfo);
    }
}

void DebugMarker::setObjectTag(uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (pfnDebugMarkerSetObjectTag) {
        VkDebugMarkerObjectTagInfoEXT tagInfo = {};
        tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
        tagInfo.objectType = objectType;
        tagInfo.object = object;
        tagInfo.tagName = name;
        tagInfo.tagSize = tagSize;
        tagInfo.pTag = tag;
        pfnDebugMarkerSetObjectTag(logicalDevice, &tagInfo);
    }
}

void DebugMarker::beginRegion(VkCommandBuffer commandBuffer, const std::string& markerName, const glm::vec4& color) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (pfnCmdDebugMarkerBegin) {
        VkDebugMarkerMarkerInfoEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        std::memcpy(markerInfo.color, glm::value_ptr(color), sizeof(glm::vec4));
        markerInfo.pMarkerName = markerName.c_str();
        pfnCmdDebugMarkerBegin(commandBuffer, &markerInfo);
    }
}

void DebugMarker::insert(VkCommandBuffer commandBuffer, const std::string& markerName, const glm::vec4& color) {
    // Check for valid function pointer (may not be present if not running in a debugging application)
    if (pfnCmdDebugMarkerInsert) {
        VkDebugMarkerMarkerInfoEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        std::memcpy(markerInfo.color, glm::value_ptr(color), sizeof(glm::vec4));
        markerInfo.pMarkerName = markerName.c_str();
        pfnCmdDebugMarkerInsert(commandBuffer, &markerInfo);
    }
}

void DebugMarker::endRegion(VkCommandBuffer commandBuffer) {
    // Check for valid function (may not be present if not running in a debugging application)
    if (pfnCmdDebugMarkerEnd) {
        pfnCmdDebugMarkerEnd(commandBuffer);
    }
}

void DebugMarker::setCommandBufferName(VkCommandBuffer commandBuffer, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(commandBuffer), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, name);
}

void DebugMarker::setQueueName(VkQueue queue, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(queue), VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT, name);
}

void DebugMarker::setImageName(VkImage image, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(image), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name);
}

void DebugMarker::setSamplerName(VkSampler sampler, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(sampler), VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT, name);
}

void DebugMarker::setBufferName(VkBuffer buffer, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(buffer), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, name);
}

void DebugMarker::setDeviceMemoryName(VkDeviceMemory memory, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(memory), VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, name);
}

void DebugMarker::setShaderModuleName(VkShaderModule shaderModule, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(shaderModule), VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, name);
}

void DebugMarker::setPipelineName(VkPipeline pipeline, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(pipeline), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT, name);
}

void DebugMarker::setPipelineLayoutName(VkPipelineLayout pipelineLayout, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(pipelineLayout), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT, name);
}

void DebugMarker::setRenderPassName(VkRenderPass renderPass, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(renderPass), VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT, name);
}

void DebugMarker::setFramebufferName(VkFramebuffer framebuffer, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(framebuffer), VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT, name);
}

void DebugMarker::setDescriptorSetLayoutName(VkDescriptorSetLayout descriptorSetLayout, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(descriptorSetLayout), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, name);
}

void DebugMarker::setDescriptorSetName(VkDescriptorSet descriptorSet, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(descriptorSet), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT, name);
}

void DebugMarker::setSemaphoreName(VkSemaphore semaphore, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(semaphore), VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, name);
}

void DebugMarker::setFenceName(VkFence fence, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(fence), VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT, name);
}

void DebugMarker::setEventName(VkEvent event, const std::string& name) {
    setObjectName(reinterpret_cast<uint64_t>(event), VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT, name);
}
