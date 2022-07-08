#pragma once

#include <volk/volk.h>

namespace fe {
    class Instance;
    class LogicalDevice;
    class PhysicalDevice;
    class Window;

    class Surface {
        friend class Graphics;
    public:
        Surface(const Instance& instance, const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Window& window);
        ~Surface();
        NONCOPYABLE(Surface);

        operator bool() const { return surface != VK_NULL_HANDLE; }
        operator const VkSurfaceKHR&() const { return surface; }

        const VkSurfaceKHR& getSurface() const { return surface; }
        const Window& getWindow() const { return window; }
        const VkSurfaceCapabilitiesKHR& getCapabilities() const;
        VkSurfaceFormatKHR getFormat() const { return format; }
        VkPresentModeKHR getPresentMode() const { return presentMode; }
        VkExtent2D getExtent() const;

    private:
        VkSurfaceFormatKHR getOptimalSurfaceFormat() const;
        VkPresentModeKHR getOptimalPresentMode() const;

        const Instance& instance;
        const PhysicalDevice& physicalDevice;
        const Window& window;

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        VkSurfaceFormatKHR format = {};
        VkPresentModeKHR presentMode{ VK_PRESENT_MODE_FIFO_KHR };
        mutable VkSurfaceCapabilitiesKHR capabilities = {};
    };
}
