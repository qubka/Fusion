#pragma once

#include "fusion/graphics/textures/texture2d.h"

namespace fe {
    struct MaterialComponent {
        glm::vec3 baseColor{ 0.5f };
        std::shared_ptr<Texture2d> diffuse;
        std::shared_ptr<Texture2d> specular;
        std::shared_ptr<Texture2d> normal;
        float shininess{ 32.0f };

        template<typename Archive>
        void load(Archive& archive) {
            uuids::uuid uuid;
            archive(cereal::make_nvp("baseColor", baseColor));
            archive(cereal::make_nvp("diffuse", uuid));
            diffuse = AssetRegistry::Get()->load<Texture2d>(uuid);
            archive(cereal::make_nvp("specular", uuid));
            specular = AssetRegistry::Get()->load<Texture2d>(uuid);
            archive(cereal::make_nvp("normal", uuid));
            normal = AssetRegistry::Get()->load<Texture2d>(uuid);
            archive(cereal::make_nvp("shininess", shininess));
        }

        template<typename Archive>
        void save(Archive& archive) const {
            uuids::uuid uuid;
            archive(cereal::make_nvp("baseColor", baseColor));
            archive(cereal::make_nvp("diffuse", diffuse && *diffuse ? diffuse->getUuid() : uuid));
            archive(cereal::make_nvp("specular", specular && *specular ? specular->getUuid() : uuid));
            archive(cereal::make_nvp("normal", normal && *normal ? normal->getUuid() : uuid));
            archive(cereal::make_nvp("shininess", shininess));
        }
    };
}