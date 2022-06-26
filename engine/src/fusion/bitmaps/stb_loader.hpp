#pragma once

#include "bitmap.hpp"

namespace fe {
    class StbLoader : public Bitmap::Registrar<StbLoader> {
        inline static const bool Registered = Register(".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm");
    public:
        static void Load(Bitmap& bitmap, const std::filesystem::path& filename);
        static void Write(const Bitmap& bitmap, const std::filesystem::path& filename);
    };
}