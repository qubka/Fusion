#pragma once

#include <volk.h>

namespace fe {
    class Bitmap {
    public:
        Bitmap() = default;
        explicit Bitmap(const std::filesystem::path& filename, int desired_channels = 0);
        explicit Bitmap(const glm::ivec2& size, int channels = 4, bool hdr = false);
        Bitmap(std::unique_ptr<uint8_t[]>&& data, const glm::ivec2& size, int channels = 4, bool hdr = false);
        ~Bitmap() = default;

        void load(const std::filesystem::path& filename, int desired_channels = 0);
        void write(const std::filesystem::path& filename) const;

        operator bool() const { return pixels.get(); }

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        VkExtent3D getExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}; }

        int getChannels() const { return channels; }
        int getBytesPerPixel() const { return channels * (hdr ? sizeof(float) : sizeof(uint8_t)); }

        int getPitch() const { return width * getBytesPerPixel(); }
        size_t getSize() const { return height * width * getBytesPerPixel(); }

        template<typename T>
        T* getData() { return reinterpret_cast<T*>(pixels.get()); }
        template<typename T>
        const T* getData() const { return reinterpret_cast<const T*>(pixels.get()); }

        bool isEmpty() const { return !pixels || width == 0 || height == 0; }
        bool isHDR() const { return hdr; }

    private:
        std::unique_ptr<uint8_t[]> pixels;
        std::filesystem::path filename;
        int width;
        int height;
        int channels;
        bool hdr;
    };
}
