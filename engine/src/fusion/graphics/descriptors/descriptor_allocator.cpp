#include "descriptor_allocator.hpp"

using namespace fe;

static const std::vector<std::pair<VkDescriptorType, float>> DESCRIPTOR_SIZES = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.0f },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.0f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.0f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.0f },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
};

DescriptorAllocator::DescriptorAllocator(VkDevice device, uint32_t count, VkDescriptorPoolCreateFlags flags) : device{device}, count{count}, flags{flags} {
}

DescriptorAllocator::~DescriptorAllocator() {
    // Delete every pool held
    for (const auto& descriptorPool : freePools) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }
    for (const auto& descriptorPool : usedPools) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }
}

VkDescriptorPool DescriptorAllocator::grabPool() const {
    if (!freePools.empty()) {
        // Grab pool from the back of the vector and remove it from there.
        VkDescriptorPool pool = freePools.back();
        freePools.pop_back();
        return pool;
    } else {
        // No pools available, so create a new one
        return createPool();
    }
}

VkDescriptorPool DescriptorAllocator::createPool() const {
    std::vector<VkDescriptorPoolSize> descriptorPools;
    descriptorPools.reserve(DESCRIPTOR_SIZES.size());

    for (const auto& [type, scalar] : DESCRIPTOR_SIZES) {
        descriptorPools.push_back({ type, static_cast<uint32_t>(scalar * static_cast<float>(count)) });
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    descriptorPoolCreateInfo.flags = flags;
    descriptorPoolCreateInfo.maxSets = count;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();

    VkDescriptorPool descriptorPool;
    VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
    return descriptorPool;
}

bool DescriptorAllocator::allocateDescriptor(VkDescriptorSetLayout layout, VkDescriptorSet& set, const void* next) const {
    // Initialize the currentPool handle if it's null
    if (!currentPool) {
        currentPool = grabPool();
        usedPools.push_back(currentPool);
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    descriptorSetAllocateInfo.pSetLayouts = &layout;
    descriptorSetAllocateInfo.descriptorPool = currentPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pNext = next;

    // Try to allocate the descriptor set
    auto result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &set);

    switch (result) {
        case VK_SUCCESS:
            // All good, return
            return true;
        case VK_ERROR_FRAGMENTED_POOL:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            // Allocate a new pool and retry
            currentPool = grabPool();
            usedPools.push_back(currentPool);

            result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &set);

            // If it still fails then we have big issues
            if (result == VK_SUCCESS)
                return true;
            break;
        default:
            // Unrecoverable error
            return false;
    }

    return false;
}

void DescriptorAllocator::resetPools() const {
    // Reset all used pools and add them to the free pools
    for (const auto& descriptorPool : usedPools) {
        vkResetDescriptorPool(device, descriptorPool, 0);
        freePools.push_back(descriptorPool);
    }

    // Clear the used pools, since we've put them all in the free pools
    usedPools.clear();

    // Reset the current pool handle back to null
    currentPool = VK_NULL_HANDLE;
}