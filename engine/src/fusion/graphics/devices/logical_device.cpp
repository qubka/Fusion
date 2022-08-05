#include "logical_device.hpp"

#include "instance.hpp"
#include "physical_device.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

const std::vector<const char*> LogicalDevice::DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
// VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME

LogicalDevice::LogicalDevice(const Instance& instance, const PhysicalDevice& physicalDevice) : instance{instance}, physicalDevice{physicalDevice} {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const float queuePriority = 0.0f;

    for (uint32_t queueFamily : physicalDevice.getUniqueFamilies()) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    /// TODO: Rework that to allow user to change enable features
    {
        auto physicalDeviceFeatures = physicalDevice.getFeatures();

        // Enable sample rate shading filtering if supported.
        if (physicalDeviceFeatures.sampleRateShading)
            enabledFeatures.sampleRateShading = VK_TRUE;

        // Fill mode non solid is required for wireframe display.
        if (physicalDeviceFeatures.fillModeNonSolid) {
            enabledFeatures.fillModeNonSolid = VK_TRUE;

            // Wide lines must be present for line width > 1.0f.
            if (physicalDeviceFeatures.wideLines)
                enabledFeatures.wideLines = VK_TRUE;
        } else
            LOG_WARNING << "Selected GPU does not support wireframe pipelines!";

        if (physicalDeviceFeatures.samplerAnisotropy)
            enabledFeatures.samplerAnisotropy = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support sampler anisotropy!";

        if (physicalDeviceFeatures.textureCompressionBC)
            enabledFeatures.textureCompressionBC = VK_TRUE;
        else if (physicalDeviceFeatures.textureCompressionASTC_LDR)
            enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
        else if (physicalDeviceFeatures.textureCompressionETC2)
            enabledFeatures.textureCompressionETC2 = VK_TRUE;

        if (physicalDeviceFeatures.vertexPipelineStoresAndAtomics)
            enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support vertex pipeline stores and atomics!";

        if (physicalDeviceFeatures.fragmentStoresAndAtomics)
            enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support fragment stores and atomics!";

        if (physicalDeviceFeatures.shaderStorageImageExtendedFormats)
            enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support shader storage extended formats!";

        if (physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat)
            enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support shader storage write without format!";

        //enabledFeatures.shaderClipDistance = VK_TRUE;
        //enabledFeatures.shaderCullDistance = VK_TRUE;

        if (physicalDeviceFeatures.geometryShader)
            enabledFeatures.geometryShader = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support geometry shaders!";

        if (physicalDeviceFeatures.tessellationShader)
            enabledFeatures.tessellationShader = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support tessellation shaders!";

        if (physicalDeviceFeatures.multiViewport)
            enabledFeatures.multiViewport = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support multi viewports!";
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    if (instance.getEnableValidationLayers()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Instance::ValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = Instance::ValidationLayers.data();
    }
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice));

    volkLoadDevice(logicalDevice);

    vkGetDeviceQueue(logicalDevice, physicalDevice.getGraphicsFamily(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getPresentFamily(), 0, &presentQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getComputeFamily(), 0, &computeQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getTransferFamily(), 0, &transferQueue);
}

LogicalDevice::~LogicalDevice() {
	//VK_CHECK(vkDeviceWaitIdle(logicalDevice));

	vkDestroyDevice(logicalDevice, nullptr);
}