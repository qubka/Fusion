#pragma once

#include "fusion/assets/asset.h"

namespace fe {
    class Texture2d;
    struct MSDFData;

    class FUSION_API Font final : public Asset {
    public:
        Font();
        explicit Font(uuids::uuid uuid, bool load = false);
        ~Font() override;

        const Texture2d* getAtlasTexture() const { return atlasTexture.get(); }
        const MSDFData* getMSDFData() const { return data.get(); }

        type_index getType() const override { return type_id<Font>; };
        uuids::uuid getUuid() const override { return uuid; };
        const std::string& getName() const override { return name; };
        const fs::path& getPath() const override { return path; };
        bool isLoaded() const override { return loaded; };
        bool isInternal() const override { return internal; };

        void load() override { loadFromFile(); };
        void unload() override { };

    private:
        void loadFromFile();

        std::unique_ptr<Texture2d> atlasTexture;
        std::unique_ptr<MSDFData> data;
        fs::path path;
        std::string name;
        uuids::uuid uuid;
        bool loaded{ false };
        bool internal{ false };
    };
}
