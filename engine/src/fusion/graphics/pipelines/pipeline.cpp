#include "pipeline.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

void Pipeline::bindPipeline(const CommandBuffer& commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
}
