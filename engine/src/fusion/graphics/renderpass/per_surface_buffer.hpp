#pragma once

#include "fence.hpp"
#include "semaphore.hpp"

namespace fe {
    template<size_t N>
    struct PerSurfaceBuffers {
        explicit PerSurfaceBuffers(uint32_t imageCount) {
            imagesInFlight.resize(imageCount, nullptr);
        }
        size_t currentFrame{ 0 };
        std::array<CommandBuffer, N> commandBuffers;
        std::array<Semaphore, N> presentCompletes;
        std::array<Semaphore, N> renderCompletes;
        std::array<Fence, N> flightFences;
        std::vector<Fence*> imagesInFlight;
    };
}