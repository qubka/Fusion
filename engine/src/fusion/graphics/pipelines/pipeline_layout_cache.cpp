#include "pipeline_layout_cache.hpp"

using namespace fe;

PipelineLayoutCache::PipelineLayoutCache(VkDevice device) : device{device} {

}

PipelineLayoutCache::~PipelineLayoutCache() {
    // Delete every pipeline layout held
    for (const auto& [info, layout] : layoutCache) {
        vkDestroyPipelineLayout(device, layout, nullptr);
    }
}

VkPipelineLayout PipelineLayoutCache::createPipelineLayout(const VkPipelineLayoutCreateInfo& info) const {
    PipelineLayoutInfo layoutInfo = {};
    layoutInfo.flags = info.flags;
    layoutInfo.setLayouts.reserve(info.setLayoutCount);
    layoutInfo.pushConstants.reserve(info.pushConstantRangeCount);

    // Copy from the direct info struct into our own one
    for (int i = 0; i < info.setLayoutCount; i++) {
        layoutInfo.setLayouts.push_back(info.pSetLayouts[i]);
    }

    bool isSorted = true;
    uint32_t lastOffset = UINT32_MAX;

    // Copy from the direct info struct into our own one
    for (const auto& [i, pushConstant] : enumerate(std::span(info.pPushConstantRanges, info.pushConstantRangeCount))) {
        layoutInfo.pushConstants.push_back(pushConstant);

        // Check that the offsets are in strict increasing order
        if (lastOffset == UINT32_MAX || pushConstant.offset > lastOffset) {
            lastOffset = pushConstant.offset;
        } else{
            isSorted = false;
        }
    }

    // Sort the bindings if they aren't in order
    if (!isSorted){
        std::sort(layoutInfo.pushConstants.begin(), layoutInfo.pushConstants.end(), [](const VkPushConstantRange& a, const VkPushConstantRange& b){
            return a.offset < b.offset;
        });
    }

    // Try to grab from cache
    if (auto it = layoutCache.find(layoutInfo); it != layoutCache.end()) {
        LOG_INFO << "Find \"VkPipelineLayout\" in cache";
        return it->second;
    } else {
        // Create a new one (not found)
        VkPipelineLayout pipelineLayout;
        VK_CHECK(vkCreatePipelineLayout(device, &info, nullptr, &pipelineLayout));

        // Add to cache
        layoutCache[layoutInfo] = pipelineLayout;
        return pipelineLayout;
    }
}

bool PipelineLayoutCache::PipelineLayoutInfo::operator==(const PipelineLayoutCache::PipelineLayoutInfo& other) const {
    if (other.flags != flags || other.setLayouts.size() != setLayouts.size() || other.pushConstants.size() != pushConstants.size()) {
        return false;
    } else {
        for (int i = 0; i < setLayouts.size(); i++) {
            if (other.setLayouts[i] != setLayouts[i]) {
                return false;
            }
        }
        // Compare each of the push constant is the same. Constants are aligned so they will match
        for (int i = 0; i < pushConstants.size(); i++) {
            if (other.pushConstants[i].stageFlags != pushConstants[i].stageFlags) {
                return false;
            }
            if (other.pushConstants[i].offset != pushConstants[i].offset) {
                return false;
            }
            if (other.pushConstants[i].size != pushConstants[i].size) {
                return false;
            }
        }
        return true;
    }
}

size_t PipelineLayoutCache::PipelineLayoutHash::operator()(const PipelineLayoutCache::PipelineLayoutInfo& i) const {
    size_t seed = 0;
    hash_combine(seed, i.flags, i.setLayouts.size(), i.pushConstants.size());
    for (const auto& l : i.setLayouts) {
        hash_combine(seed, l);
    }
    for (const auto& p : i.pushConstants) {
        hash_combine(seed, p.stageFlags, p.offset, p.size);
    }
    return seed;
}
