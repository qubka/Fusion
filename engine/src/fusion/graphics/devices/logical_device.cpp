#include "logical_device.hpp"

#include "instance.hpp"
#include "physical_device.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

const std::vector<const char*> LogicalDevice::DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        //VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
};

LogicalDevice::LogicalDevice(const Instance& instance, const PhysicalDevice& physicalDevice) : instance{instance}, physicalDevice{physicalDevice} {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const float queuePriority = 0.0f;

    for (uint32_t queueFamily : physicalDevice.getUniqueFamilies()) {
        auto& queueCreateInfo = queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
    }

    enabledFeatures = physicalDevice.getFeatures();

    // Enable required extension features
    physicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    physicalDeviceDescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
    void* pNextChain = &physicalDeviceDescriptorIndexingFeatures;

    /// TODO: Rework that to allow user to change enable features
    {
        // Enable sample rate shading filtering if supported.
        if (enabledFeatures.sampleRateShading)
            enabledFeatures.sampleRateShading = VK_TRUE;

        // Fill mode non solid is required for wireframe display.
        if (enabledFeatures.fillModeNonSolid) {
            enabledFeatures.fillModeNonSolid = VK_TRUE;

            // Wide lines must be present for line width > 1.0f.
            if (enabledFeatures.wideLines)
                enabledFeatures.wideLines = VK_TRUE;
        } else
            LOG_WARNING << "Selected GPU does not support wireframe pipelines!";

        if (enabledFeatures.samplerAnisotropy)
            enabledFeatures.samplerAnisotropy = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support sampler anisotropy!";

        if (enabledFeatures.textureCompressionBC)
            enabledFeatures.textureCompressionBC = VK_TRUE;
        else if (enabledFeatures.textureCompressionASTC_LDR)
            enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
        else if (enabledFeatures.textureCompressionETC2)
            enabledFeatures.textureCompressionETC2 = VK_TRUE;

        if (enabledFeatures.vertexPipelineStoresAndAtomics)
            enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support vertex pipeline stores and atomics!";

        if (enabledFeatures.fragmentStoresAndAtomics)
            enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support fragment stores and atomics!";

        if (enabledFeatures.shaderStorageImageExtendedFormats)
            enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support shader storage extended formats!";

        if (enabledFeatures.shaderStorageImageWriteWithoutFormat)
            enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support shader storage write without format!";

        //enabledFeatures.shaderClipDistance = VK_TRUE;
        //enabledFeatures.shaderCullDistance = VK_TRUE;

        if (enabledFeatures.geometryShader)
            enabledFeatures.geometryShader = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support geometry shaders!";

        if (enabledFeatures.tessellationShader)
            enabledFeatures.tessellationShader = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support tessellation shaders!";

        if (enabledFeatures.multiViewport)
            enabledFeatures.multiViewport = VK_TRUE;
        else
            LOG_WARNING << "Selected GPU does not support multi viewports!";
    }

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    if (instance.getEnableValidationLayers()) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Instance::ValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = Instance::ValidationLayers.data();
    }

    if (pNextChain) {
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        physicalDeviceFeatures2.features = enabledFeatures;
        physicalDeviceFeatures2.pNext = pNextChain;
        deviceCreateInfo.pEnabledFeatures = VK_NULL_HANDLE;
        deviceCreateInfo.pNext = &physicalDeviceFeatures2;
    } else {
        deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    }
    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice));

    volkLoadDevice(logicalDevice);

    vkGetDeviceQueue(logicalDevice, physicalDevice.getGraphicsFamily(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getPresentFamily(), 0, &presentQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getComputeFamily(), 0, &computeQueue);
    vkGetDeviceQueue(logicalDevice, physicalDevice.getTransferFamily(), 0, &transferQueue);
}

LogicalDevice::~LogicalDevice() {
	vkDestroyDevice(logicalDevice, nullptr);
}