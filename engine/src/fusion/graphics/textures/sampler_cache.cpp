#include "sampler_cache.hpp"

using namespace fe;

SamplerCache::SamplerCache(VkDevice device) : device{device} {

}

SamplerCache::~SamplerCache() {
    // Delete every sampler layout held
    for (const auto& [info, sampler] : samplerCache) {
        vkDestroySampler(device, sampler, nullptr);
    }
}

VkSampler SamplerCache::createSampler(const VkSamplerCreateInfo& info) const {
    // Try to grab from cache
    if (auto it = samplerCache.find(info); it != samplerCache.end()) {
        LOG_INFO << "Find \"VkSampler\" in cache";
        return it->second;
    } else {
        // Create a new one (not found)
        VkSampler sampler;
        VK_CHECK(vkCreateSampler(device, &info, nullptr, &sampler));

        // Add to cache
        samplerCache[info] = sampler;
        return sampler;
    }
}

size_t SamplerCache::SamplerHash::operator()(const VkSamplerCreateInfo& i) const {
    size_t seed = 0;
    hash_combine(seed,
                 i.sType,
                 i.pNext,
                 i.flags,
                 i.magFilter,
                 i.minFilter,
                 i.mipmapMode,
                 i.addressModeU,
                 i.addressModeV,
                 i.addressModeW,
                 i.mipLodBias,
                 i.anisotropyEnable,
                 i.maxAnisotropy,
                 i.compareEnable,
                 i.compareOp,
                 i.minLod,
                 i.maxLod,
                 i.borderColor,
                 i.unnormalizedCoordinates);
    return seed;
}
