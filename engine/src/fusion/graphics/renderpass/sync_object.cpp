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

void SyncObject::init(VkDevice device) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore));
    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFence));
}

void SyncObject::destroy(VkDevice device) {
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    imageInFlight = VK_NULL_HANDLE;
}

void SyncObject::reset() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    destroy(logicalDevice);
    init(logicalDevice);
}