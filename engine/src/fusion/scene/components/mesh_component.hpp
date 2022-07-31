#pragma once

#include "fusion/assets/asset_registry.hpp"

#include "fusion/models/mesh.hpp"
#include "fusion/models/model.hpp"

namespace fe {
    struct MeshComponent {
        std::shared_ptr<Mesh> runtime;

        /*Model& operator*() { return *model; }
        const Model& operator*() const { return *model; }
        operator Model() const { return *model; }*/

        //fs::path path;
        //bool castShadows{ false };

        template<typename Archive>
        void load(Archive& archive) {
            fs::path path;
            uint32_t index;
            archive(cereal::make_nvp("path", path),
                    cereal::make_nvp("index", index));

            if (auto model = AssetRegistry::Get()->get_or_emplace<Model>(path, path)) {
                runtime = model->getMesh(index);
            } else {
                LOG_ERROR << "Asset not found: \"" << path << "\"";
            }
        }

        template<typename Archive>
        void save(Archive& archive) const {
            if (runtime) {
                archive(cereal::make_nvp("path", runtime->getPath()),
                        cereal::make_nvp("index", runtime->getMeshIndex()));
            } else {
                archive(cereal::make_nvp("path", ""),
                        cereal::make_nvp("index", ""));
            }
        }
    };
}