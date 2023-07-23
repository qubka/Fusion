#include "semaphore.h"

#include "fusion/graphics/graphics.h"

using namespace fe;

Semaphore::Semaphore() {
    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphore));
}

Semaphore::Semaphore(VkSemaphore semaphore) : semaphore{semaphore} {
}

Semaphore::~Semaphore() {
    if (semaphore != VK_NULL_HANDLE) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        vkDestroySemaphore(logicalDevice, semaphore, nullptr);
    }
}