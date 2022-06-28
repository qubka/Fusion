#include "sync_object.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

SyncObject::SyncObject() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    init(logicalDevice);
}

SyncObject::~SyncObject() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    destroy(logicalDevice);
}

void SyncObject::init(const VkDevice& logicalDevice) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
    Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
    Graphics::CheckVk(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &inFlightFence));
}

void SyncObject::destroy(const VkDevice& logicalDevice) {
    vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
    vkDestroyFence(logicalDevice, inFlightFence, nullptr);
    imageInFlight = VK_NULL_HANDLE;
}

void SyncObject::reset() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    destroy(logicalDevice);
    init(logicalDevice);
}