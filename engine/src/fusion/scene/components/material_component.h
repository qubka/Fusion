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
            archive(cereal::make_nvp("baseColor", baseColor));
            uuids::uuid diffuseUuid;
            archive(cereal::make_nvp("diffuse", diffuseUuid));
            diffuse = AssetRegistry::Get()->load<Texture2d>(diffuseUuid);
            uuids::uuid specularUuid;
            archive(cereal::make_nvp("specular", specularUuid));
            specular = AssetRegistry::Get()->load<Texture2d>(specularUuid);
            uuids::uuid normalUuid;
            archive(cereal::make_nvp("normal", normalUuid));
            normal = AssetRegistry::Get()->load<Texture2d>(normalUuid);
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