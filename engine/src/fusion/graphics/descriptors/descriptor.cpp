#include "descriptor.hpp"

#include "fusion/graphics/textures/texture.hpp"

using namespace fe;

WriteDescriptorSet::WriteDescriptorSet(const VkWriteDescriptorSet& writeDescriptorSet, const std::vector<const Descriptor*>& descriptors)
        : writeDescriptorSet{writeDescriptorSet} {
    imageInfos.reserve(descriptors.size());
    for (const auto descriptor : descriptors) {
        imageInfos.push_back((reinterpret_cast<const Texture*>(descriptor))->getDescriptor());
    }
    this->writeDescriptorSet.descriptorCount = static_cast<uint32_t>(descriptors.size());
    this->writeDescriptorSet.pImageInfo = imageInfos.data();
}