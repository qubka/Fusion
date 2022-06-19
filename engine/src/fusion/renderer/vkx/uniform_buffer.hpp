#pragma once

#include "buffer.hpp"
#include "context.hpp"

#include <vector>

namespace vkx {
    template<typename T, std::size_t N>
    class UniformBuffer {
    public:
        void create(const vkx::Context& context) {
            /* Global Uniform Buffer Object */
            for (int i = 0; i < buffers.size(); i++) {
                buffers[i] = context.createUniformBuffer(T{});
            }
        }

        void destroy() {
            for (auto& buffer : buffers) {
                buffer.destroy();
            }
        }

        size_t size() const { return buffers.size(); }
        vkx::Buffer& operator[](uint32_t index) { return buffers[index]; }
        const vkx::Buffer& operator[](uint32_t index) const { return buffers[index]; }

    private:
        std::array<vkx::Buffer, N> buffers;
    };
}
