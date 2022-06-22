#pragma once

#include "fusion/renderer/descriptors/descriptor_set.hpp"
#include "fusion/renderer/buffers/uniform_handler.hpp"
#include "fusion/renderer/buffers/storage_handler.hpp"
#include "fusion/renderer/buffers/push_handler.hpp"
#include "fusion/renderer/pipelines/shader.hpp"

namespace fe {
    /**
     * @brief Class that handles a descriptor set.
     */
    class DescriptorsHandler {
    public:
        DescriptorsHandler() = default;
        explicit DescriptorsHandler(const Pipeline& pipeline);

        template<typename T>
        void push(const std::string& descriptorName, const T* descriptor, const std::optional<OffsetSize>& offsetSize = std::nullopt) {
            if (!shader)
                return;

            // Finds the local value given to the descriptor name.
            auto it = descriptors.find(descriptorName);

            if (it != descriptors.end()) {
                // If the descriptor and size have not changed then the write is not modified.
                if (it->second.descriptor == descriptor && it->second.offsetSize == offsetSize) {
                    return;
                }

                descriptors.erase(it);
            }

            // Only non-null descriptors can be mapped.
            if (!descriptor) {
                return;
            }

            // When adding the descriptor find the location in the shader.
            auto location = shader->getDescriptorLocation(descriptorName);

            if (!location) {
    #ifdef ACID_DEBUG
                if (shader->ReportedNotFound(descriptorName, true)) {
                    Log::Error("Could not find descriptor in shader ", shader->GetName(), " of name ", std::quoted(descriptorName), '\n');
                }
    #endif

                return;
            }

            auto descriptorType = shader->getDescriptorType(*location);

            if (!descriptorType) {
    #ifdef ACID_DEBUG
                if (shader->ReportedNotFound(descriptorName, true)) {
                    Log::Error("Could not find descriptor in shader ", shader->GetName(), " of name ", std::quoted(descriptorName), " at location ", *location, '\n');
                }
    #endif
                return;
            }

            // Adds the new descriptor value.
            auto writeDescriptor = descriptor->getWriteDescriptor(*location, *descriptorType, offsetSize);
            descriptors.emplace(descriptorName, DescriptorValue{descriptor, std::move(writeDescriptor), offsetSize, *location});
            changed = true;
        }

        template<typename T>
        void push(const std::string& descriptorName, const T* descriptor, WriteDescriptorSet writeDescriptorSet) {
            if (!shader)
                return;

            if (auto it = descriptors.find(descriptorName); it != descriptors.end()) {
                descriptors.erase(it);
            }

            auto location = shader->getDescriptorLocation(descriptorName);
            //auto descriptorType = shader->GetDescriptorType(*location);

            descriptors.emplace(descriptorName, DescriptorValue{descriptor, std::move(writeDescriptorSet), std::nullopt, *location});
            changed = true;
        }

        void push(const std::string& descriptorName, UniformHandler& uniformHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);
        void push(const std::string& descriptorName, StorageHandler& storageHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);
        void push(const std::string& descriptorName, PushHandler& pushHandler, const std::optional<OffsetSize>& offsetSize = std::nullopt);

        bool update(const Pipeline& pipeline);

        void bindDescriptor(const CommandBuffer& commandBuffer, const Pipeline &pipeline);

        const DescriptorSet* getDescriptorSet() const { return descriptorSet.get(); }

    private:
        class DescriptorValue {
        public:
            const Descriptor* descriptor;
            WriteDescriptorSet writeDescriptor;
            std::optional<OffsetSize> offsetSize;
            uint32_t location;
        };

        const Shader* shader;
        bool pushDescriptors = false;
        std::unique_ptr<DescriptorSet> descriptorSet;

        std::map<std::string, DescriptorValue> descriptors;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        bool changed = false;
    };
}
