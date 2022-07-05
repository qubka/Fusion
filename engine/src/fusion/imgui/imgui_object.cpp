#include "imgui_object.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/vku.hpp"

#include <volk.h>
#include <imgui/imgui.h>

using namespace fe;

void ImGuiObject::cmdRender(const CommandBuffer& commandBuffer) {
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
        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
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

    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdLists = drawData->CmdLists[i];
        for (const auto& cmd : cmdLists->CmdBuffer) {
            VkRect2D scissor = vku::rect2D(
                    glm::uvec2{cmd.ClipRect.z - cmd.ClipRect.x, cmd.ClipRect.w - cmd.ClipRect.y}, // extent
                    glm::ivec2{std::max(static_cast<int>((cmd.ClipRect.x)), 0), std::max(static_cast<int>((cmd.ClipRect.y)), 0)} // offset
            );
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            // Bind DescriptorSet with font or user texture
            //VkDescriptorSet descriptor[1] = { static_cast<VkDescriptorSet>(pcmd.TextureId) };
            //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, descriptor, 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, cmd.ElemCount, 1, indexOffset, vertexOffset, 0);
            indexOffset += static_cast<int32_t>(cmd.ElemCount);
        }
        vertexOffset += cmdLists->VtxBuffer.Size;
    }
}