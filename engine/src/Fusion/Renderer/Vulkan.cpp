#include "Vulkan.hpp"
#include "Window.hpp"

using namespace Fusion;

// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    FE_CORE_ERROR << "[Vulkan] Validation layer: " << pCallbackData->pMessage;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

Vulkan::Vulkan(Window& window) : window{window} {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
}

Vulkan::~Vulkan() {
    instance.destroySurfaceKHR(surface, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
    }

    device.destroyCommandPool(commandPool, nullptr);

    device.destroy(nullptr);
    instance.destroy(nullptr);
}

void Vulkan::createInstance() {
    if (enableValidationLayers) {
        FE_ASSERT(checkValidationLayerSupport() && "validation layers requested, but not available!");
    }

    uint32_t version;
    vkEnumerateInstanceVersion(&version);

    FE_CORE_DEBUG << "System support vulkan variant: " << VK_API_VERSION_VARIANT(version)
              << ", Major: " << VK_API_VERSION_MAJOR(version)
              << ", Minor: " << VK_API_VERSION_MINOR(version)
              << ", Patch: " << VK_API_VERSION_PATCH(version) << '\n';

    version = VK_MAKE_VERSION(1, 0, 0);

    auto appInfo = vk::ApplicationInfo{
            window.getTitle().c_str(),
            version,
            "Fusion",
            version,
            VK_API_VERSION_1_0
    };

    hasGflwRequiredInstanceExtensions();

    auto extensions = getRequiredExtensions();

    auto createInfo = vk::InstanceCreateInfo{
            vk::InstanceCreateFlags(),
            &appInfo,
            0,
            nullptr,
            static_cast<uint32_t>(extensions.size()),
            extensions.data()
    };

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    auto result = vk::createInstance(&createInfo, nullptr, &instance);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create vulkan!");
}

std::vector<const char*> Vulkan::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool Vulkan::checkValidationLayerSupport() const {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void Vulkan::hasGflwRequiredInstanceExtensions() const {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    FE_CORE_DEBUG << "available extensions:";
    std::unordered_set<std::string> available;
    for (const auto& extension : extensions) {
        FE_CORE_DEBUG << "\t\"" << extension.extensionName << "\"";
        available.insert(extension.extensionName);
    }

    FE_CORE_DEBUG << "required extensions:";
    auto requiredExtensions = getRequiredExtensions();
    for (const auto& required : requiredExtensions) {
        FE_CORE_DEBUG << "\t\"" << required << "\"";
        FE_ASSERT(available.find(required) != available.end() && "missing required glfw extension");
    }
}

void Vulkan::setupDebugMessenger() {
    if (!enableValidationLayers)
        return;

    //auto dldi = vk::DispatchLoaderDynamic(*instance, vkGetInstanceProcAddr);

    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        debugCallback
    );

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    //instance.createDebugUtilsMessengerEXT(createInfo);
    //instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);

    // NOTE: reinterpret_cast is also used by vulkan.hpp internally for all these structs
    CreateDebugUtilsMessengerEXT(instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &callback);
    FE_ASSERT(callback && "failed to set up debug callback!");
}

void Vulkan::pickPhysicalDevice() {
    uint32_t deviceCount;
    instance.enumeratePhysicalDevices(&deviceCount, nullptr);
    FE_ASSERT(deviceCount && "failed to find GPUs with Vulkan support!");

    std::vector<vk::PhysicalDevice> devices(deviceCount);
    instance.enumeratePhysicalDevices(&deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    FE_ASSERT(physicalDevice && "failed to find a suitable GPU!");

    vk::PhysicalDeviceProperties props;
    physicalDevice.getProperties(&props);
    FE_CORE_DEBUG << "physical device found: " << props.deviceName;
}

bool Vulkan::isDeviceSuitable(const vk::PhysicalDevice& device) const {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate{};
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Vulkan::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const {
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    uint32_t extensionCount;
    device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
    device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Vulkan::findQueueFamilies(const vk::PhysicalDevice& device) const {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport;
        device.getSurfaceSupportKHR(i, surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void Vulkan::createLogicalDevice() {
    QueueFamilyIndices indices = findPhysicalQueueFamilies();
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(uniqueQueueFamilies.size());

    float queuePriority{1.0f};

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        queueCreateInfos.emplace_back(
           vk::DeviceQueueCreateFlags(),
           queueFamily,
           1, // queueCount
           &queuePriority
        );
    }

    auto deviceFeatures = vk::PhysicalDeviceFeatures();
    auto createInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data()
    );
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    auto result = physicalDevice.createDevice(&createInfo, nullptr, &device);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create logical device!");

    device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
    device.getQueue(indices.presentFamily.value(), 0, &presentQueue);
}

void Vulkan::createSurface() {
    VkSurfaceKHR raw;
    glfwCreateWindowSurface(instance, window, nullptr, &raw);
    FE_ASSERT(raw && "failed to create window surface!");
    surface = raw;
}

SwapChainSupportDetails Vulkan::querySwapChainSupport(const vk::PhysicalDevice& device) const {
    SwapChainSupportDetails details;
    device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

    uint32_t formatCount;
    device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        device.getSurfacePresentModesKHR(
                surface,
                &presentModeCount,
                details.presentModes.data());
    }

    return details;
}

void Vulkan::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    auto result = device.createCommandPool(&poolInfo, nullptr, &commandPool);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create command pool!");
}

vk::Format Vulkan::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const {
    for (const auto& format : candidates) {
        vk::FormatProperties props;
        physicalDevice.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features ||
            tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    FE_ASSERT(false && "failed to find supported format!");
    return vk::Format::eUndefined;
}

uint32_t Vulkan::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    vk::PhysicalDeviceMemoryProperties memProperties;
    physicalDevice.getMemoryProperties(&memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    FE_ASSERT(false && "failed to find suitable memory type!");
    return 0;
}

void Vulkan::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) const {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    auto result = device.createBuffer(&bufferInfo, nullptr, &buffer);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create buffer!");

    vk::MemoryRequirements memRequirements;
    device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = device.allocateMemory(&allocInfo, nullptr, &bufferMemory);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to allocateDescriptor buffer memory!");

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void Vulkan::copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size) const {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void Vulkan::copyBufferToImage(const vk::Buffer& buffer, const vk::Image& image, uint32_t width, uint32_t height, uint32_t layerCount) const {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;

    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{width, height, 1};

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void Vulkan::submit(std::function<void(vk::CommandBuffer& cmd)>&& function) const {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    //execute the function
    function(commandBuffer);

    endSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer Vulkan::beginSingleTimeCommands() const {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    device.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(&beginInfo);

    return commandBuffer;
}

void Vulkan::endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();

    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void Vulkan::transitionImageLayout(const vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlagBits sourceStage;
    vk::PipelineStageFlagBits destinationStage;

    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    } else {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;

    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

    } else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;

    } else {
        FE_ASSERT(false && "unsupported layout transition!");
        return;
    }

    commandBuffer.pipelineBarrier(sourceStage,
                                  destinationStage,
                                  {},
                                  0,
                                  nullptr,
                                  0,
                                  nullptr,
                                  1,
                                  &barrier);

    endSingleTimeCommands(commandBuffer);
}

void Vulkan::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) const {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    auto result = device.createImage(&imageInfo, nullptr, &image);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create image!");

    vk::MemoryRequirements memRequirements;
    device.getImageMemoryRequirements(image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = device.allocateMemory(&allocInfo, nullptr, &imageMemory);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to allocate descriptor's image memory!");

    device.bindImageMemory(image, imageMemory, 0);
}

void Vulkan::createImageView(const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView& view) const {
    vk::ImageViewCreateInfo createInfo{};
    createInfo.image = image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = format;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    auto result = device.createImageView(&createInfo, nullptr, &view);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create image views!");
}

void Vulkan::createSampler(vk::Filter magFilter, vk::Filter minFilter, vk::SamplerAddressMode addressMode, vk::SamplerMipmapMode minmapMode, vk::Sampler& sampler) const {
    vk::PhysicalDeviceProperties props;
    physicalDevice.getProperties(&props);

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = minmapMode;

    auto result = device.createSampler(&samplerInfo, nullptr, &sampler);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create texture sampler!");
}

bool Vulkan::hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}