#include "logical_device.hpp"

#include "fusion/graphics/graphics.hpp"

#include "instance.hpp"
#include "physical_device.hpp"

using namespace fe;

const std::vector<const char *> LogicalDevice::DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME

LogicalDevice::LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice) :
	instance(instance),
	physicalDevice(physicalDevice) {
	createQueueIndices();
	createLogicalDevice();
}

LogicalDevice::~LogicalDevice() {
	Graphics::CheckVk(vkDeviceWaitIdle(logicalDevice));

	vkDestroyDevice(logicalDevice, nullptr);
}

void LogicalDevice::createQueueIndices() {
	uint32_t deviceQueueFamilyPropertyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &deviceQueueFamilyPropertyCount, nullptr);
	std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &deviceQueueFamilyPropertyCount, deviceQueueFamilyProperties.data());

	for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++) {
        VkQueueFlags flags = deviceQueueFamilyProperties[i].queueFlags;
		// Check for graphics support.
		if (flags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
			supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
		}

		// Check for presentation support.
		//VkBool32 presentSupport;
		//vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &presentSupport);

		if (deviceQueueFamilyProperties[i].queueCount > 0 /*&& presentSupport*/) {
			presentFamily = i;
		}

		// Check for compute support.
		if (flags & VK_QUEUE_COMPUTE_BIT) {
			computeFamily = i;
			supportedQueues |= VK_QUEUE_COMPUTE_BIT;
		}

		// Check for transfer support.
		if (flags & VK_QUEUE_TRANSFER_BIT) {
			transferFamily = i;
			supportedQueues |= VK_QUEUE_TRANSFER_BIT;
		}

        // Stop if found all required indices
		if (graphicsFamily != VK_QUEUE_FAMILY_IGNORED &&
            presentFamily != VK_QUEUE_FAMILY_IGNORED &&
            computeFamily != VK_QUEUE_FAMILY_IGNORED &&
            transferFamily != VK_QUEUE_FAMILY_IGNORED) {
			break;
		}
	}
}

void LogicalDevice::createLogicalDevice() {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	const float defaultQueuePriority = 0.0;

	if (supportedQueues & VK_QUEUE_GRAPHICS_BIT) {
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = graphicsFamily;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(graphicsQueueCreateInfo);
	} else {
        throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
	}

    // Dedicated compute queue
	if (supportedQueues & VK_QUEUE_COMPUTE_BIT && computeFamily != graphicsFamily) {
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.queueFamilyIndex = computeFamily;
		computeQueueCreateInfo.queueCount = 1;
		computeQueueCreateInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(computeQueueCreateInfo);
	} else {
        // Else we use the same queue.
		computeFamily = graphicsFamily;
	}

    // Dedicated transfer queue.
	if (supportedQueues & VK_QUEUE_TRANSFER_BIT && transferFamily != graphicsFamily && transferFamily != computeFamily) {
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.queueFamilyIndex = transferFamily;
		transferQueueCreateInfo.queueCount = 1;
		transferQueueCreateInfo.pQueuePriorities = &defaultQueuePriority;
		queueCreateInfos.push_back(transferQueueCreateInfo);
	} else {
        // Else we use the same queue.
		transferFamily = graphicsFamily;
	}

    // TODO: Rework that to allow user to change enable features
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
        else LOG_WARNING << "Selected GPU does not support tessellation shaders!";

        if (physicalDeviceFeatures.multiViewport)
            enabledFeatures.multiViewport = VK_TRUE;
        else LOG_WARNING << "Selected GPU does not support multi viewports!";
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
	Graphics::CheckVk(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice));

	volkLoadDevice(logicalDevice);

	vkGetDeviceQueue(logicalDevice, graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, presentFamily, 0, &presentQueue);
	vkGetDeviceQueue(logicalDevice, computeFamily, 0, &computeQueue);
	vkGetDeviceQueue(logicalDevice, transferFamily, 0, &transferQueue);
}
