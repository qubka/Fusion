#pragma once

namespace fe {
    class FUSION_API OffsetSize {
    public:
        OffsetSize(uint32_t offset, uint32_t size) : offset{offset}, size{size} {}
        uint32_t getOffset() const { return offset; }
        uint32_t getSize() const { return size; }

        bool operator==(const OffsetSize& rhs) const {
            return offset == rhs.offset && size == rhs.size;
        }

        bool operator!=(const OffsetSize& rhs) const {
            return !operator==(rhs);
        }

    private:
        uint32_t offset;
        uint32_t size;
    };

    class Descriptor;

    class FUSION_API WriteDescriptorSet {
    public:
        WriteDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet, const VkDescriptorImageInfo& imageInfo)
                : writeDescriptorSet{writeDescriptorSet}
                , imageInfos{imageInfo} {
            this->writeDescriptorSet.pImageInfo = imageInfos.data();
        }

        WriteDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet, const VkDescriptorBufferInfo& bufferInfo)
                : writeDescriptorSet{writeDescriptorSet}
                , bufferInfos{bufferInfo} {
            this->writeDescriptorSet.pBufferInfo = bufferInfos.data();
        }

        WriteDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet, const std::vector<const Descriptor*>& descriptors);

        const VkWriteDescriptorSet& getWriteDescriptorSet() const { return writeDescriptorSet; }

    private:
        VkWriteDescriptorSet writeDescriptorSet;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
    };

    class FUSION_API Descriptor {
    public:
        Descriptor() = default;
        virtual ~Descriptor() = default;

        virtual WriteDescriptorSet getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const = 0;
    };
}
