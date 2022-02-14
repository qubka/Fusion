#include "VulkanTools.hpp"

void vk::tools::InsertImageMemoryBarrier(
        vk::CommandBuffer& commandBuffer,
        vk::Image& image,
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags dstAccessMask,
        vk::ImageLayout oldImageLayout,
        vk::ImageLayout newImageLayout,
        vk::PipelineStageFlags srcStageMask,
        vk::PipelineStageFlags dstStageMask,
        vk::ImageSubresourceRange subresourceRange)
{
    vk::ImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    commandBuffer.pipelineBarrier(
            srcStageMask,
            dstStageMask,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageMemoryBarrier);
}