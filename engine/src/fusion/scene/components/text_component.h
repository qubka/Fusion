#pragma once

#include "fusion/text/font.h"

namespace fe {
    struct TextComponent {
        std::string text;
        std::shared_ptr<Font> font;
        glm::vec4 color{ 1.0f };
        float kerning{ 0.0f };
        float lineSpacing{ 0.0f };

        template<typename Archive>
        void load(Archive& archive) {
            archive(cereal::make_nvp("text", text));
            uuids::uuid uuid;
            archive(cereal::make_nvp("font", uuid));
            font = AssetRegistry::Get()->load<Font>(uuid);
            archive(cereal::make_nvp("color", color));
            archive(cereal::make_nvp("kerning", kerning));
            archive(cereal::make_nvp("lineSpacing", lineSpacing));
        }

        template<typename Archive>
        void save(Archive& archive) const {
            uuids::uuid uuid;
            archive(cereal::make_nvp("text", text));
            archive(cereal::make_nvp("font", font ? font->getUuid() : uuids::uuid{}));
            archive(cereal::make_nvp("color", color));
            archive(cereal::make_nvp("kerning", kerning));
            archive(cereal::make_nvp("lineSpacing", lineSpacing));
        }
    };
}