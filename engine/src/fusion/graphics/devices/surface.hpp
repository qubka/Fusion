#pragma once

#include <volk.h>

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

        operator const VkSurfaceKHR&() const { return surface; }

        const VkSurfaceKHR& getSurface() const { return surface; }
        const Window& getWindow() const { return window; }

        const VkSurfaceCapabilitiesKHR& getCapabilities() const { return capabilities; }
        const std::vector<VkSurfaceFormatKHR>& getFormat() const { return formats; }
        const std::vector<VkPresentModeKHR>& getPresentModes() const { return presentModes; }

    private:
        const Instance& instance;
        const Window& window;

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        VkSurfaceCapabilitiesKHR capabilities =  {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}
