#pragma once

#include "fusion/graphics/graphics.hpp"

#include <volk.h>

namespace fe {
    class Semaphore {
    public:
        Semaphore() {
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
            Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphore));
        }
        Semaphore(VkSemaphore semaphore) : semaphore{semaphore} {}
        ~Semaphore() {
            if (semaphore != VK_NULL_HANDLE) {
                const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
                vkDestroySemaphore(logicalDevice, semaphore, nullptr);
            }
        }

        operator bool() const { return semaphore != VK_NULL_HANDLE; }
        operator const VkSemaphore&() const { return semaphore; }
        const VkSemaphore& getSemaphore() const { return semaphore; }

    private:
        VkSemaphore semaphore{ VK_NULL_HANDLE };
    };
}
