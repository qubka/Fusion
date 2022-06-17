#pragma once

namespace fe {
    class Image {
    public:
        static std::shared_ptr<Image> fromFile(const std::string& filename, int channels = 0, bool flip = false);

        int width() const { return width_; }
        int height() const { return height_; }
        int channels() const { return channels_; }
        int bytesPerPixel() const { return channels_ * (hdr_ ? sizeof(float) : sizeof(unsigned char)); }
        int pitch() const { return width_ * bytesPerPixel(); }

        bool empty() const { return !pixels_ || width_ == 0 || height_ == 0; }
        bool HDR() const { return hdr_; }

        template<typename T>
        const T* pixels() const {
            return reinterpret_cast<const T*>(pixels_.get());
        }

    private:
        Image() = default;

        int width_{ 0 };
        int height_{ 0 };
        int channels_{ 0 };
        bool hdr_{ false };

        std::unique_ptr<unsigned char> pixels_;
    };
}
