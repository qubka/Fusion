#pragma once

#include "fusion/graphics/descriptors/descriptor.hpp"
#include "fusion/graphics/descriptors/descriptor_set.hpp"
#include "fusion/graphics/pipelines/shader.hpp"

namespace fe {
    class UniformHandler;
    class StorageHandler;
    class PushHandler;
    /**
     * @brief Class that handles a descriptor set.
     */
    class DescriptorsHandler {
    public:
        DescriptorsHandler() = default;
        explicit DescriptorsHandler(const Pipeline& pipeline);

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Descriptor*>>>
        void push(const std::string& descriptorName, const T* descriptor, const std::optional<OffsetSize>& offsetSize = std::nullopt) {
            if (!shader)
                return;

            // Finds the local value given to the descriptor name
            if (auto it = descriptors.find(descriptorName); it != descriptors.end()) {
                // If the descriptor and size have not changed then the write is not modified
                if (it->second.descriptor == descriptor && it->second.offsetSize == offsetSize) {
                    return;
                }
                descriptors.erase(it);
            }

            // Only non-null descriptors can be mapped
            if (!descriptor) {
                return;
            }

            // When adding the descriptor find the location in the shader
            auto location = shader->getDescriptorLocation(descriptorName);

            if (!location) {
    #if FUSION_DEBUG
                if (shader->reportedNotFound(descriptorName, true)) {
                    LOG_ERROR << "Could not find descriptor in shader \"" << shader->getName() << "\" of name \"" << descriptorName << "\"";
                }
    #endif
                return;
            }

            auto descriptorType = shader->getDescriptorType(*location);

            if (!descriptorType) {
    #if FUSION_DEBUG
                if (shader->reportedNotFound(descriptorName, true)) {
                    LOG_ERROR << "Could not find descriptor in shader \"" << shader->getName() << "\" of name \"" << descriptorName << "\" at location " << *location;
                }
    #endif
                return;
            }

            // Adds the new descriptor value
            auto writeDescriptor = descriptor->getWriteDescriptor(*location, *descriptorType, offsetSize);
            descriptors.emplace(descriptorName, DescriptorValue{descriptor, std::move(writeDescriptor), offsetSize, *location});
            changed = true;
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Descriptor*>>>
        void push(const std::string& descriptorName, const T* descriptor, WriteDescriptorSet writeDescriptorSet) {
            if (!shader)
                return;

            auto location = shader->getDescriptorLocation(descriptorName);

            descriptors[descriptorName] = DescriptorValue{descriptor, std::move(writeDescriptorSet), std::nullopt, *location};

            changed = true;
        }

        void push(const std::string& descriptorName, UniformHandler& uniformHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);
        void push(const std::string& descriptorName, StorageHandler& storageHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);
        void push(const std::string& descriptorName, PushHandler& pushHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);

        bool update(const Pipeline& pipeline);

        void bindDescriptor(const CommandBuffer& commandBuffer, const Pipeline &pipeline);

        const DescriptorSet* getDescriptorSet() const { return descriptorSet.get(); }

    private:
        struct DescriptorValue {
            const Descriptor* descriptor;
            WriteDescriptorSet writeDescriptor;
            std::optional<OffsetSize> offsetSize;
            uint32_t location;
        };

        const Shader* shader{ nullptr };
        std::unique_ptr<DescriptorSet> descriptorSet;
        std::flat_map<std::string, DescriptorValue> descriptors;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        bool pushDescriptors{ false };
        bool changed{ false };
    };
}
