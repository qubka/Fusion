#include "texture_depth.h"

using namespace fe;

static const std::vector<VkFormat> TRY_FORMATS = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
};

TextureDepth::TextureDepth(const glm::uvec2& extent, VkSampleCountFlagBits samples)
        : Texture{VK_FILTER_LINEAR,
                  VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                  samples,
                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_DEPTH_BIT,
                  VK_IMAGE_VIEW_TYPE_2D,
                  FindSupportedFormat(TRY_FORMATS, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT),
                  1,
                  1,
                  { extent.x, extent.y, 1 },
                  false,
                  false} {
	if (format == VK_FORMAT_UNDEFINED)
		throw std::runtime_error("No depth stencil format could be selected");
}
