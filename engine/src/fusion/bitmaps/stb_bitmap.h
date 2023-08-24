#pragma once

#include "bitmap.h"

namespace fe {
    class FUSION_API StbBitmap : public Bitmap::Registrar<StbBitmap> {
        //inline static const bool Registered = Register(".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm");
    public:
        static void Load(Bitmap& bitmap, const fs::path& filepath);
        static void Write(const Bitmap& bitmap, const fs::path& filepath);
    };
}
