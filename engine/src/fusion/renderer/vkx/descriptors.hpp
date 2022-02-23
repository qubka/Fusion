#pragma once

#include "context.hpp"

namespace vkx {
    class DescriptorAllocator {
    public:
        struct PoolSizes {
            std::vector<std::pair<vk::DescriptorType, float>> sizes =
                {
                    { vk::DescriptorType::eSampler, 0.5f },
                    { vk::DescriptorType::eCombinedImageSampler, 4.f },
                    { vk::DescriptorType::eSampledImage, 4.f },
                    { vk::DescriptorType::eStorageImage, 1.f },
                    { vk::DescriptorType::eUniformTexelBuffer, 1.f },
                    { vk::DescriptorType::eStorageTexelBuffer, 1.f },
                    { vk::DescriptorType::eUniformBuffer, 2.f },
                    { vk::DescriptorType::eStorageBuffer, 2.f },
                    { vk::DescriptorType::eUniformBufferDynamic, 1.f },
                    { vk::DescriptorType::eStorageBufferDynamic, 1.f },
                    { vk::DescriptorType::eInputAttachment, 0.5f }
                };
        };

        DescriptorAllocator(const vk::Device& device) : device{device} {};

        vk::DescriptorPool grabPool();
        void resetPools();
        bool allocateDescriptor(const vk::DescriptorSetLayout& layout, vk::DescriptorSet& set);
        void updateDescriptor(std::vector<vk::WriteDescriptorSet>& writes) const;
        void destroy();

    private:
        vk::DescriptorPool createPool(uint32_t count, vk::DescriptorPoolCreateFlags flags) const;

        vk::DescriptorPool currentPool{ nullptr };
        PoolSizes descriptorSizes;
        std::vector<vk::DescriptorPool> usedPools;
        std::vector<vk::DescriptorPool> freePools;

        const vk::Device& device;

        static constexpr int INITIAL_POOL_SIZE = 1000;
    };

    class DescriptorLayoutCache {
    public:
        DescriptorLayoutCache(const vk::Device& device) : device{device} {};

        vk::DescriptorSetLayout createDescriptorLayout(vk::DescriptorSetLayoutCreateInfo& info);
        void destroy();

        struct DescriptorLayoutInfo {
            //good idea to turn this into an inlined array
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            bool operator==(const DescriptorLayoutInfo& other) const;
            size_t hash() const;
        };

    private:
        struct DescriptorLayoutHash {
            std::size_t operator()(const DescriptorLayoutInfo& k) const { return k.hash(); }
        };

        std::unordered_map<DescriptorLayoutInfo, vk::DescriptorSetLayout, DescriptorLayoutHash> layoutCache;
        const vk::Device& device;
    };

    class DescriptorBuilder {
    public:
        DescriptorBuilder(DescriptorLayoutCache& cache, DescriptorAllocator& allocator)
            : cache{cache}, allocator{allocator} {};

        DescriptorBuilder& bindBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);
        DescriptorBuilder& bindImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        bool build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        bool build(vk::DescriptorSet& set);
    private:

        std::vector<vk::WriteDescriptorSet> writes;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        DescriptorLayoutCache& cache;
        DescriptorAllocator& allocator;
    };
};
