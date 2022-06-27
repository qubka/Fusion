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

        operator bool() const { return surface != VK_NULL_HANDLE; }
        operator const VkSurfaceKHR&() const { return surface; }

        struct SupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };
        SupportDetails getSupportDetails() const;

        const VkSurfaceKHR& getSurface() const { return surface; }
        const Window& getWindow() const { return window; }

    private:
        const Instance& instance;
        const PhysicalDevice& physicalDevice;
        const Window& window;

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
    };
}
