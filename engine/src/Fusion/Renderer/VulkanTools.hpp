#pragma once

#include "Vulkan.hpp"

namespace vk {
    class tools {
    public:
        static void InsertImageMemoryBarrier(
                vk::CommandBuffer& commandBuffer,
                vk::Image& image,
                vk::AccessFlags srcAccessMask,
                vk::AccessFlags dstAccessMask,
                vk::ImageLayout oldImageLayout,
                vk::ImageLayout newImageLayout,
                vk::PipelineStageFlags srcStageMask,
                vk::PipelineStageFlags dstStageMask,
                vk::ImageSubresourceRange subresourceRange);
    };
}