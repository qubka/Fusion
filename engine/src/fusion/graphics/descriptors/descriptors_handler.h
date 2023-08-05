#pragma once

#include "fusion/graphics/descriptors/descriptor.h"
#include "fusion/graphics/descriptors/descriptor_set.h"
#include "fusion/graphics/pipelines/shader.h"

namespace fe {
    class Image;
    class UniformHandler;
    class StorageHandler;
    class PushHandler;
    /**
     * @brief Class that handles a descriptor set.
     */
    class FUSION_API DescriptorsHandler {
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
                if (it->second.descriptors.front() == descriptor && it->second.offsetSize == offsetSize) {
                    return;
                }
                descriptors.erase(it);
            }

            // Only non-null descriptors can be mapped
            if (!descriptor)
                return;

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
            descriptors.emplace(descriptorName, DescriptorValue{std::vector{reinterpret_cast<const Descriptor*>(descriptor)}, std::move(writeDescriptor), offsetSize, *location});
            changed = true;
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Descriptor*>>>
        void push(const std::string& descriptorName, const std::vector<const T*>& descriptor) {
            if (!shader)
                return;

            // Finds the local value given to the descriptor name
            if (auto it = descriptors.find(descriptorName); it != descriptors.end()) {
                // If the descriptor and size have not changed then the write is not modified
                if (it->second.descriptors.size() == descriptor.size() && it->second.descriptors == descriptor) {
                    return;
                }
                descriptors.erase(it);
            }

            // Only non-null descriptors can be mapped
            if (descriptors.empty())
                return;

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

            //auto writeDescriptor = descriptor->getWriteDescriptor(*location, *descriptorType, offsetSize);
            VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
            descriptorWrite.dstBinding = *location;
            descriptorWrite.dstArrayElement = 0;
            //descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = *descriptorType;
            //descriptorWrite.pImageInfo = &imageInfo;

            descriptors.emplace(descriptorName, DescriptorValue{descriptor, WriteDescriptorSet{descriptorWrite, descriptor}, std::nullopt, *location});
            changed = true;
        }

        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Descriptor*>>>
        void push(const std::string& descriptorName, const T* descriptor, WriteDescriptorSet writeDescriptorSet) {
            if (!shader)
                return;

            auto location = shader->getDescriptorLocation(descriptorName);
            if (!location) {
#if FUSION_DEBUG
                if (shader->reportedNotFound(descriptorName, true)) {
                    LOG_ERROR << "Could not find descriptor in shader \"" << shader->getName() << "\" of name \"" << descriptorName << "\"";
                }
#endif
                return;
            }

            descriptors[descriptorName] = DescriptorValue{std::vector{reinterpret_cast<const Descriptor*>(descriptor)}, std::move(writeDescriptorSet), std::nullopt, *location};
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
            std::vector<const Descriptor*> descriptors;
            WriteDescriptorSet writeDescriptor;
            std::optional<OffsetSize> offsetSize;
            uint32_t location;
        };

        const Shader* shader{ nullptr };
        std::unique_ptr<DescriptorSet> descriptorSet;
        fst::unordered_flatmap<std::string, DescriptorValue> descriptors;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        bool changed{ false };
    };
}
