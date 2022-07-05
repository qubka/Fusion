#pragma once

#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/buffers/buffer.hpp"

namespace fe {
    class CommandBuffer;
    class ImGuiObject {
    public:
        ImGuiObject() = default;
        ~ImGuiObject() = default;
        NONCOPYABLE(ImGuiObject);

        void cmdRender(const CommandBuffer& commandBuffer);

        const Image2d* getFont() const { return font.get(); }
        void setFont(std::unique_ptr<Image2d>&& font) { this->font = std::move(font); }

    private:
        std::unique_ptr<Image2d> font;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        std::queue<std::unique_ptr<Buffer>> removePool;
    };
}
