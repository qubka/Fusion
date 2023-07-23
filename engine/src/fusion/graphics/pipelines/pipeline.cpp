#include "pipeline.h"

#include "fusion/graphics/commands/command_buffer.h"

using namespace fe;

void Pipeline::bindPipeline(const CommandBuffer& commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
}
