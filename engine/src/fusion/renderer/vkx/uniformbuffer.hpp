#pragma once

#include "buffer.hpp"
#include "context.hpp"

#include <vector>

namespace vkx {
    template<typename T, std::size_t N>
    struct UniformBuffer {
        std::array<vkx::Buffer, N> buffers;

        void destroy() {
            for (auto& buffer : buffers) {
                buffer.destroy();
            }
        }

        void create(const vkx::Context& context) {
            /* Global Uniform Buffer Object */
            for (int i = 0; i < buffers.size(); i++) {
                buffers[i] = context.createUniformBuffer(T{});
            }
        }

        vkx::Buffer& operator[](uint32_t i) { return buffers[i]; }
    };
}
