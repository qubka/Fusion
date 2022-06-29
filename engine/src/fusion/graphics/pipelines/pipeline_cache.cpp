#include "pipeline_cache.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

PipelineCache::PipelineCache(const LogicalDevice& logicalDevice) : logicalDevice{logicalDevice} {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK(vkCreatePipelineCache(logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache));
}

PipelineCache::~PipelineCache() {
    vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);
}