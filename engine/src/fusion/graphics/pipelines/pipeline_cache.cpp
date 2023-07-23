#include "pipeline_cache.h"

#include "fusion/graphics/graphics.h"

using namespace fe;

PipelineCache::PipelineCache(VkDevice device) : device{device} {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
    VK_CHECK(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache));
}

PipelineCache::~PipelineCache() {
    vkDestroyPipelineCache(device, pipelineCache, nullptr);
}