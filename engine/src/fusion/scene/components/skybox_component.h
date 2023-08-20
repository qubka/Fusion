#pragma once

#include "fusion/assets/asset_registry.h"

#include "fusion/graphics/textures/texture_cube.h"

namespace fe {
    struct SkyboxComponent {
        std::shared_ptr<TextureCube> texture;

        template<typename Archive>
        void load(Archive& archive) {
            uuids::uuid uuid;
            archive(cereal::make_nvp("texture", uuid));
            texture = AssetRegistry::Get()->load<TextureCube>(uuid);
        }

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("texture", texture ? texture->getUuid() : uuids::uuid{}));
        }
    };
}