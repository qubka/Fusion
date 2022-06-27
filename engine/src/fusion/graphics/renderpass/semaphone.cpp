#include "semaphore.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

Semaphore::Semaphore() {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphore));
}

Semaphore::Semaphore(VkSemaphore semaphore) : semaphore{semaphore} {

}

Semaphore::~Semaphore() {
    if (semaphore != VK_NULL_HANDLE) {
        const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
        vkDestroySemaphore(logicalDevice, semaphore, nullptr);
    }
}