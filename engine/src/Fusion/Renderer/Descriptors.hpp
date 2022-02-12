#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    class FUSION_API DescriptorPool {
    public:
        class Builder {
        public:
            explicit Builder(Vulkan& vulkan) : vulkan{vulkan} {}

            Builder& addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(vk::DescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<DescriptorPool> build() const;

        private:
            Vulkan& vulkan;
            std::vector<vk::DescriptorPoolSize> poolSizes;
            vk::DescriptorPoolCreateFlags poolFlags;
            uint32_t maxSets{1000};

            friend class DescriptorPool;
        };

        DescriptorPool(const Builder& builder);
        ~DescriptorPool();

        bool allocateDescriptor(const vk::DescriptorSetLayout& setLayout, vk::DescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<vk::DescriptorSet>& descriptors) const;
        void resetPool();

    private:
        Vulkan& vulkan;
        vk::DescriptorPool descriptorPool;

        friend class DescriptorWriter;
    };

    class FUSION_API DescriptorLayout {
    public:
        class Builder {
        public:
            explicit Builder(Vulkan& vulkan) : vulkan{vulkan} {}
            Builder& addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<DescriptorLayout> build() const;

        private:
            Vulkan& vulkan;
            std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

            friend class DescriptorLayout;
        };

        DescriptorLayout(const Builder& builder);
        ~DescriptorLayout();

        const vk::DescriptorSetLayout& getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Vulkan& vulkan;
        vk::DescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

        friend class DescriptorWriter;
    };

    class FUSION_API DescriptorWriter {
    public:
        DescriptorWriter(DescriptorLayout& layout, DescriptorPool& pool);
        DescriptorWriter& writeBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo);
        DescriptorWriter& writeImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo);

        bool build(vk::DescriptorSet& set);
        void overwrite(vk::DescriptorSet& set);

    private:
        DescriptorLayout& layout;
        DescriptorPool& pool;
        std::vector<vk::WriteDescriptorSet> writes;
    };

    /*class FUSION_API DescriptorAllocator {
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

        DescriptorAllocator(Vulkan& vulkan);
        ~DescriptorAllocator();

        vk::DescriptorPool grabPool();
        void resetPools();
        bool allocateDescriptor(const vk::DescriptorSetLayout& layout, vk::DescriptorSet& set);
        void updateDescriptor(std::vector<vk::WriteDescriptorSet>& writes) const;

    private:
        vk::DescriptorPool createPool(uint32_t count, vk::DescriptorPoolCreateFlags flags) const;

        vk::DescriptorPool currentPool{nullptr};
        PoolSizes descriptorSizes;
        std::vector<vk::DescriptorPool> usedPools;
        std::vector<vk::DescriptorPool> freePools;

        Vulkan& vulkan;
    };

    class FUSION_API DescriptorLayoutCache {
    public:
        DescriptorLayoutCache(Vulkan& vulkan);
        ~DescriptorLayoutCache();

        vk::DescriptorSetLayout createDescriptorLayout(vk::DescriptorSetLayoutCreateInfo& info);

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
        Vulkan& vulkan;
    };

    class FUSION_API DescriptorBuilder {
    public:
        DescriptorBuilder(DescriptorLayoutCache& cache, DescriptorAllocator& allocator);
        ~DescriptorBuilder();

        DescriptorBuilder& bindBuffer(uint32_t binding, vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);
        DescriptorBuilder& bindImage(uint32_t binding, vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        bool build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        bool build(vk::DescriptorSet& set);
    private:

        std::vector<vk::WriteDescriptorSet> writes;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        DescriptorLayoutCache& cache;
        DescriptorAllocator& allocator;
    };*/
};
