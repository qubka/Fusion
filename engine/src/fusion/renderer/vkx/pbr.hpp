#pragma once

#include "context.hpp"
#include "texture.hpp"
#include "model.hpp"

namespace vkx { namespace pbr {
    // Generate a BRDF integration map used as a look-up-table (stores roughness / NdotV)
    void generateBRDFLUT(const vkx::Context& context, vkx::Texture2D& target);
    // Generate an irradiance cube map from the environment cube map
    void generateIrradianceCube(const vkx::Context& context,
                                vkx::Texture& target,
                                const vkx::Model& skybox,
                                const vkx::VertexLayout& vertexLayout,
                                const vk::DescriptorImageInfo& skyboxDescriptor);
    // Prefilter environment cubemap
    // See https://placeholderart.wordpress.com/2015/07/28/implementation-notes-runtime-environment-map-filtering-for-image-based-lighting/
    void generatePrefilteredCube(const vkx::Context& context,
                                 vkx::Texture& target,
                                 const vkx::Model& skybox,
                                 const vkx::VertexLayout& vertexLayout,
                                 const vk::DescriptorImageInfo& skyboxDescriptor);
}}  // namespace vkx::pbr
