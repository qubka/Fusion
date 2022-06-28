#include "sync_object.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

SyncObject::SyncObject() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore));
    Graphics::CheckVk(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore));
    Graphics::CheckVk(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFence));
}

SyncObject::~SyncObject() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
    vkDestroyFence(logicalDevice, inFlightFence, nullptr);
    imageInFlight = VK_NULL_HANDLE;
}
