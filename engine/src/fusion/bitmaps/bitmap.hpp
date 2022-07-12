#pragma once

#include "bitmap_factory.hpp"

#include <volk/volk.h>

namespace fe {
    enum class BitmapChannels { Default = 0, Grey = 1, GreyAlpha, Rgb, RgbAlpha };

    class Bitmap : public BitmapFactory<Bitmap> {
        friend class StbLoader;
        friend class GliLoader;
    public:
        Bitmap() = default;
        explicit Bitmap(const fs::path& filepath);
        explicit Bitmap(const glm::uvec2& size, BitmapChannels channels = BitmapChannels::RgbAlpha, bool hdr = false);
        Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::uvec2& size, BitmapChannels channels = BitmapChannels::RgbAlpha, bool hdr = false);
        ~Bitmap() override = default;
        NONCOPYABLE(Bitmap);

        void load(const fs::path& filepath);
        void write(const fs::path& filepath) const;

        operator bool() const { return data.operator bool(); }

        int getWidth() const { return static_cast<int>(size.x); }
        int getHeight() const { return static_cast<int>(size.y); }
        const glm::uvec2 getSize() const { return size; }
        VkExtent3D getExtent() const { return {size.x, size.y, 1}; }
        BitmapChannels getChannels() const { return channels; }
        uint8_t getBytesPerPixel() const { return static_cast<uint8_t>(channels) * (hdr ? sizeof(float) : sizeof(uint8_t)); }
        uint32_t getPitch() const { return size.x * getBytesPerPixel(); }
        size_t getLength() const { return size.x * size.y * getBytesPerPixel(); }
        const fs::path& getFilePath() const { return filePath; }

        template<typename T>
        T* getData() { return reinterpret_cast<T*>(data.get()); }
        template<typename T>
        const T* getData() const { return reinterpret_cast<const T*>(data.get()); }

        bool isEmpty() const { return !data || size.x == 0 || size.y == 0; }
        bool isHDR() const { return hdr; }

    private:
        std::unique_ptr<uint8_t[]> data;
        fs::path filePath;
        glm::uvec2 size{ 0 };
        BitmapChannels channels{ BitmapChannels::RgbAlpha };
        bool hdr{ false };

    private:
        static size_t CalculateLength(const glm::uvec2& size, BitmapChannels channels = BitmapChannels::RgbAlpha, bool hdr = false);
    };
}
