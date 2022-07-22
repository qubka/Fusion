#include "imgui_object.hpp"

#include "fusion/graphics/vku.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/imgui/imgui_utils.hpp"

#include <volk/volk.h>
#include <imgui/imgui.h>

using namespace fe;

void ImGuiObject::cmdRender(const CommandBuffer& commandBuffer, const Pipeline& pipeline, std::map<ImTextureID, VkDescriptorSet>& descriptorSets) {
    ImDrawData* drawData = ImGui::GetDrawData();

    // Note: Alignment is done inside buffer creation
    VkDeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if (vertexBufferSize == 0 || indexBufferSize == 0)
        return;

    // Vertex buffer
    if (!vertexBuffer || (vertexCount != drawData->TotalVtxCount)) {
        if (vertexBuffer) {
            removePool.push(std::move(vertexBuffer));
        }
        vertexBuffer = std::make_unique<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vertexBuffer->map();
        vertexCount = drawData->TotalVtxCount;
    }

    // Index buffer
    if (!indexBuffer || (indexCount != drawData->TotalIdxCount)) {
        if (indexBuffer) {
            removePool.push(std::move(indexBuffer));
        }
        indexBuffer = std::make_unique<Buffer>(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        indexBuffer->map();
        indexCount = drawData->TotalIdxCount;
    }

    // Remove unused buffers from pool
    while (removePool.size() > MAX_FRAMES_IN_FLIGHT * 2) {
        removePool.pop();
    }

    // Upload data
    auto vtxDst = reinterpret_cast<ImDrawVert*>(vertexBuffer->getMappedMemory());
    auto idxDst = reinterpret_cast<ImDrawIdx*>(indexBuffer->getMappedMemory());

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdList = drawData->CmdLists[i];
        std::memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        std::memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmdList->VtxBuffer.Size;
        idxDst += cmdList->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
    vertexBuffer->flush();
    indexBuffer->flush();

    // Bind and draw current buffers
    VkBuffer vertexBuffers[1] = { *vertexBuffer };
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 size{ drawData->DisplaySize * drawData->FramebufferScale };
    ImVec2 clipOff{ drawData->DisplayPos };         // (0,0) unless using multi-viewports
    ImVec2 clipScale{ drawData->FramebufferScale }; // (1,1) unless using retina display which are often (2,2)

    VkDescriptorSet lastSet = VK_NULL_HANDLE;
    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdLists = drawData->CmdLists[i];
        for (const auto& cmd : cmdLists->CmdBuffer) {
            if (cmd.TextureId) {
                auto& descSet = descriptorSets[cmd.TextureId];
                if (lastSet != descSet) {
                    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &descSet, 0, nullptr);
                    lastSet = descSet;
                }
            }

            // Project scissor/clipping rectangles into framebuffer space
            glm::vec2 clipRectMin{ (cmd.ClipRect.x - clipOff.x) * clipScale.x, (cmd.ClipRect.y - clipOff.y) * clipScale.y };
            glm::vec2 clipRectMax{ (cmd.ClipRect.z - clipOff.x) * clipScale.x, (cmd.ClipRect.w - clipOff.y) * clipScale.y };

            // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
            if (clipRectMin.x < 0.0f) clipRectMin.x = 0.0f;
            if (clipRectMin.y < 0.0f) clipRectMin.y = 0.0f;
            if (clipRectMax.x > size.x) clipRectMax.x = size.x;
            if (clipRectMax.y > size.y) clipRectMax.y = size.y;
            if (clipRectMax.x < clipRectMin.x || clipRectMax.y < clipRectMin.y)
                continue;

            // Apply scissor/clipping rectangle
            VkRect2D scissor{ vku::rect2D(clipRectMax - clipRectMin, clipRectMin) };
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            // Draw buffer
            vkCmdDrawIndexed(commandBuffer, cmd.ElemCount, 1, cmd.IdxOffset + indexOffset, cmd.VtxOffset + vertexOffset, 0);
        }

        indexOffset += cmdLists->IdxBuffer.Size;
        vertexOffset += cmdLists->VtxBuffer.Size;
    }
}