#pragma once

#include "bitmap.hpp"

namespace fe {
    class GliLoader : public Bitmap::Registrar<GliLoader> {
        //inline static const bool Registered = Register(".ktx", ".kmg", ".dds");
    public:
        static void Load(Bitmap& bitmap, const fs::path& filepath);
        static void Write(const Bitmap& bitmap, const fs::path& filepath);
    };
}
