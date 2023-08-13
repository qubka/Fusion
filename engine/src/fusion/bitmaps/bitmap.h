#pragma once

#include "bitmap_factory.h"

namespace fe {
    class FUSION_API Bitmap : public BitmapFactory<Bitmap> {
        friend class StbToolbox;
        friend class GliToolbox;
    public:
        Bitmap() = default;
        explicit Bitmap(const fs::path& filepath);
        explicit Bitmap(const glm::uvec2& size, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
        Bitmap(std::unique_ptr<std::byte[]>&& data, const glm::uvec2& size, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
        ~Bitmap() override = default;
        NONCOPYABLE(Bitmap); // TEMP

        void load(const fs::path& filepath);
        void write(const fs::path& filepath) const;

        operator bool() const { return data.operator bool(); }

        template<typename T>
        operator gsl::span<const T>() const { return { getData<T>(), getLength() }; }

        int getWidth() const { return static_cast<int>(size.x); }
        int getHeight() const { return static_cast<int>(size.y); }
        const glm::uvec2& getSize() const { return size; }
        glm::uvec3 getExtent() const { return { size.x, size.y, 1 }; }
        VkFormat getFormat() const { return format; }
        size_t getLength() const { return size.x * size.y * vku::get_format_params(format).bytes; }
        const fs::path& getPath() const { return path; }

        template<typename T>
        T* getData() { return reinterpret_cast<T*>(data.get()); }
        template<typename T>
        const T* getData() const { return reinterpret_cast<const T*>(data.get()); }

        bool isEmpty() const { return !data || (size.x == 0 && size.y == 0); }

    private:
        std::unique_ptr<std::byte[]> data;
        glm::uvec2 size{ 0 };
        VkFormat format{ VK_FORMAT_UNDEFINED };
        fs::path path;
    };
}
