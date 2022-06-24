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
        const VkSurfaceCapabilitiesKHR& getCapabilities() const { return capabilities; }
        const VkSurfaceFormatKHR& getFormat() const { return format; }

    private:
        const Instance& instance;
        const PhysicalDevice& physicalDevice;
        const LogicalDevice& logicalDevice;
        const Window& window;

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        VkSurfaceCapabilitiesKHR capabilities{};
        VkSurfaceFormatKHR format{};
    };
}
