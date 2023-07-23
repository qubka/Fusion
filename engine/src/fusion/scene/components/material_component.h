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
            fs::path diffusePath;
            archive(cereal::make_nvp("diffuse", diffusePath));
            diffuse = AssetRegistry::Get()->load<Texture2d>(diffusePath);
            fs::path specularPath;
            archive(cereal::make_nvp("specular", specularPath));
            specular = AssetRegistry::Get()->load<Texture2d>(specularPath);
            fs::path normalPath;
            archive(cereal::make_nvp("normal", normalPath));
            normal = AssetRegistry::Get()->load<Texture2d>(normalPath);
            archive(cereal::make_nvp("shininess", shininess));
        }

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("baseColor", baseColor));
            archive(cereal::make_nvp("diffuse", diffuse->getPath()));
            archive(cereal::make_nvp("specular", specular->getPath()));
            archive(cereal::make_nvp("normal", normal->getPath()));
            archive(cereal::make_nvp("shininess", shininess));
        }
    };
}