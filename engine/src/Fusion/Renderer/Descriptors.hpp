#pragma once

#include "Device.hpp"

namespace Fusion {
    class FUSION_API DescriptorPool {
    public:
        class Builder {
        public:
            explicit Builder(Device& device) : device{device} {}

            Builder& addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(vk::DescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<DescriptorPool> build() const;

        private:
            Device& device;
            std::vector<vk::DescriptorPoolSize> poolSizes;
            vk::DescriptorPoolCreateFlags poolFlags;
            uint32_t maxSets{1000};

            friend class DescriptorPool;
        };

        DescriptorPool(const Builder& builder);
        ~DescriptorPool();
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool(DescriptorPool&&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&&) = delete;

        bool allocateDescriptor(const vk::DescriptorSetLayout& setLayout, vk::DescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<vk::DescriptorSet>& descriptors) const;
        void resetPool();

    private:
        Device& device;
        vk::DescriptorPool descriptorPool;

        friend class DescriptorWriter;
    };

    class FUSION_API DescriptorLayout {
    public:
        class Builder {
        public:
            explicit Builder(Device& device) : device{device} {}
            Builder& addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<DescriptorLayout> build() const;

        private:
            Device& device;
            std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

            friend class DescriptorLayout;
        };

        DescriptorLayout(const Builder& builder);
        ~DescriptorLayout();
        DescriptorLayout(const DescriptorLayout&) = delete;
        DescriptorLayout(DescriptorLayout&&) = delete;
        DescriptorLayout& operator=(const DescriptorLayout&) = delete;
        DescriptorLayout& operator=(DescriptorLayout&&) = delete;

        const vk::DescriptorSetLayout& getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device& device;
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
};
