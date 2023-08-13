#pragma once

#include "fusion/assets/asset_registry.h"

#include "fusion/models/mesh.h"
#include "fusion/models/model.h"

namespace fe {
    struct MeshComponent {
        std::shared_ptr<Model> model;
        uint32_t index;

        const Mesh* get() const { return (model && *model) ? model->getMesh(index) : nullptr; }

        template<typename Archive>
        void load(Archive& archive) {
            uuids::uuid modelUuid;
            archive(cereal::make_nvp("model", modelUuid));
            model = AssetRegistry::Get()->load<Model>(modelUuid);
            archive(cereal::make_nvp("index", index));
        }

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("model", model->getUuid()));
            archive(cereal::make_nvp("index", index));
        }
    };
}