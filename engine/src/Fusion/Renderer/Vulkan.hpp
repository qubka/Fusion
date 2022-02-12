#pragma once

#include "vulkan/vulkan.hpp"

namespace Fusion {
    struct FUSION_API SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct FUSION_API QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class Window;

    class FUSION_API Vulkan {
#ifdef FS_DEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    public:
        Vulkan(Window& window);
        ~Vulkan();
        Vulkan(const Vulkan&) = delete;
        Vulkan(Vulkan&&) = delete;
        Vulkan& operator=(const Vulkan&) = delete;
        Vulkan& operator=(Vulkan&&) = delete;

        const vk::Instance& getInstance() const { return instance; }
        const vk::Device& getDevice() const { return device; };
        const vk::PhysicalDevice& getPhysical() const { return physicalDevice; };
        const vk::SurfaceKHR& getSurface() const { return surface; };
        const vk::Queue& getGraphicsQueue() const { return graphicsQueue; };
        const vk::Queue& getPresentQueue() const { return presentQueue; };
        const vk::CommandPool& getCommandPool() const { return commandPool; };
        const Window& getWindow() const { return window; }

        SwapChainSupportDetails getSwapChainSupport() const { return querySwapChainSupport(physicalDevice); };
        QueueFamilyIndices findPhysicalQueueFamilies() const { return findQueueFamilies(physicalDevice); };

        vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;

        void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) const;
        void copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, vk::DeviceSize size) const;
        void copyBufferToImage(const vk::Buffer& buffer, const vk::Image& image, uint32_t width, uint32_t height, uint32_t layerCount) const;
        void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) const;
        void transitionImageLayout(const vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
        void createImageView(const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView& view) const;

        void submit(std::function<void(vk::CommandBuffer& cmd)>&& function) const;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        std::vector<const char*> getRequiredExtensions() const;
        bool checkValidationLayerSupport() const;
        void hasGflwRequiredInstanceExtensions() const;
        bool isDeviceSuitable(const vk::PhysicalDevice& device) const;
        bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
        QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
        SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;

        vk::CommandBuffer beginSingleTimeCommands() const;
        void endSingleTimeCommands(const vk::CommandBuffer& commandBuffer) const;
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

        static bool hasStencilComponent(vk::Format format);

        Window& window;

        vk::Instance instance;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::SurfaceKHR surface;
        vk::CommandPool commandPool;

        VkDebugUtilsMessengerEXT callback{nullptr};

        const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}