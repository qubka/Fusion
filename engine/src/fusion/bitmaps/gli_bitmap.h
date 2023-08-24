#pragma once

#include "bitmap.h"

namespace fe {
    class FUSION_API GliBitmap : public Bitmap::Registrar<GliBitmap> {
        //inline static const bool Registered = Register(".ktx", ".kmg", ".dds");
    public:
        static void Load(Bitmap& bitmap, const fs::path& filepath);
        static void Write(const Bitmap& bitmap, const fs::path& filepath);
    };
}
