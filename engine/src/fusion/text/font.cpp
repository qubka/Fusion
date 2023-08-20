#include "font.h"
#include "msdf.h"

#include "fusion/core/engine.h"
#include "fusion/assets/asset_registry.h"
#include "fusion/graphics/textures/texture2d.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/GlyphGeometry.h>

using namespace fe;

template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
static std::unique_ptr<Texture2d> CreateAndCacheAtlas(gsl::span<const msdf_atlas::GlyphGeometry> glyphs, int width, int height) {
    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.overlapSupport = true;
    attributes.scanlinePass = true;

    msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator{width, height};
    generator.setAttributes(attributes);
    generator.setThreadCount(8);
    generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

    auto storage = static_cast<msdfgen::BitmapConstRef<T, N>>(generator.atlasStorage());
    auto size = storage.height * storage.width;
    auto pixels = gsl::make_span(storage.pixels, size * N);

    if constexpr (N == 1)
        return std::make_unique<Texture2d>(pixels, glm::uvec2{storage.width, storage.height}, VK_FORMAT_R8_UNORM);
    else if constexpr (N == 2)
        return std::make_shared<Texture2d>(pixels, glm::uvec2{storage.width, storage.height}, VK_FORMAT_R8G8_UNORM);
    else if constexpr (N == 3) {
        // VK_FORMAT_R8G8B8_UNORM not supported by many devices !
        std::vector<uint8_t> buffer(size * 4);
        vku::rgb_to_rgba(buffer.data(), pixels.data(), size);
        return std::make_unique<Texture2d>(buffer, glm::uvec2{storage.width, storage.height}, VK_FORMAT_R8G8B8A8_UNORM);
    } else {
        return std::make_unique<Texture2d>(pixels, glm::uvec2{storage.width, storage.height}, VK_FORMAT_R8G8B8A8_UNORM);
    }

}

Font::Font() : data{std::make_unique<MSDFData>()} {
}

Font::Font(uuids::uuid uuid, bool load) : uuid{uuid}, data{std::make_unique<MSDFData>()} {
    if (load)
        loadFromFile();
}

Font::~Font() {
}

void Font::loadFromFile() {
    if (loaded) {
        FE_LOG_DEBUG("Font: '{}' already was loaded", path);
        return;
    }

    auto op = AssetRegistry::Get()->getDatabase()->getValue(uuid);
    if (!op.has_value()) {
        FE_LOG_ERROR("Font: [{}] is not valid", uuid);
        return;
    }

    path = std::move(*op);
    name = path.filename().string();

    fs::path filepath{ Engine::Get()->getApp()->getProjectSettings().projectRoot / path }; // get full path

    // TODO: Move freetype out
    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    FE_ASSERT(ft);

#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif

    /*msdfgen::FontHandle* font = nullptr;
    FileSystem::ReadBytes(filepath, [&ft, &font](gsl::span<const uint8_t> buffer) {
        font = msdfgen::loadFontData(ft, buffer.data(), static_cast<int>(buffer.size()));
    });*/
    msdfgen::FontHandle* font = msdfgen::loadFont(ft, filepath.string().c_str());

    if (!font)
        throw std::runtime_error("Font is empty");

#if FUSION_DEBUG
    FE_LOG_DEBUG("Font '{}' loaded in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif

    using CharsetRange = std::pair<uint32_t, uint32_t>;

    // From imgui_draw.cpp
    static const CharsetRange charsetRanges[] = {
        { 0x0020, 0x00FF }
    };

    msdf_atlas::Charset charset;
    for (CharsetRange range : charsetRanges) {
        for (uint32_t c = range.first; c <= range.second; c++)
            charset.add(c);
    }

    double fontScale = 1.0;
    data->fontGeometry = msdf_atlas::FontGeometry(&data->glyphs);
    int glyphsLoaded = data->fontGeometry.loadCharset(font, fontScale, charset);
    FE_LOG_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

    double emSize = 40.0;

    msdf_atlas::TightAtlasPacker atlasPacker;
    // atlasPacker.setDimensionsConstraint()
    atlasPacker.setPixelRange(2.0);
    atlasPacker.setMiterLimit(1.0);
    atlasPacker.setPadding(0);
    atlasPacker.setScale(emSize);
    int remaining = atlasPacker.pack(data->glyphs.data(), static_cast<int>(data->glyphs.size()));
    FE_ASSERT(remaining == 0);

    int width, height;
    atlasPacker.getDimensions(width, height);
    emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
    // if MSDF || MTSDF

    uint64_t coloringSeed = 0;
    static constexpr bool expensiveColoring = false;
    if constexpr (expensiveColoring) {
        msdf_atlas::Workload([&glyphs = data->glyphs, &coloringSeed](int i, int threadNo) -> bool {
            uint64_t glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
            glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
            return true;
        }, static_cast<int>(data->glyphs.size())).finish(THREAD_COUNT);
    } else {
        uint64_t glyphSeed = coloringSeed;
        for (msdf_atlas::GlyphGeometry& glyph : data->glyphs) {
            glyphSeed *= LCG_MULTIPLIER;
            glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
        }
    }

    atlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(data->glyphs, width, height);

    msdfgen::destroyFont(font);
    msdfgen::deinitializeFreetype(ft);

    loaded = true;
}