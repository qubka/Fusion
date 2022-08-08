#include "sync_object.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

SyncObject::SyncObject() {
    init();
}

SyncObject::~SyncObject() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    destroy();
}

void SyncObject::init() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
    VK_CHECK(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &inFlightFence));
}

void SyncObject::destroy() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
    vkDestroyFence(logicalDevice, inFlightFence, nullptr);
    imageInFlight = VK_NULL_HANDLE;
}

void SyncObject::reset() {
    destroy();
    init();
}