#include "RendererSystemBase.hpp"

using namespace Fusion;

size_t RendererSystemBase::drawCalls{};
size_t RendererSystemBase::totalCalls{};
size_t RendererSystemBase::totalVertices{};

void RendererSystemBase::ResetStats() {
    drawCalls = 0;
    totalCalls = 0;
    totalVertices = 0;
}
