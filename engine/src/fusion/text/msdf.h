#pragma once

#include <msdf-atlas-gen/msdf-atlas-gen.h>

namespace fe {
    struct MSDFData {
        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry fontGeometry;
    };
}