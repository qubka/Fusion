#pragma once

//#include "fusion/models/model.hpp"

#include <cereal/cereal.hpp>

namespace fe {
    class Model;
    struct ModelComponent {
        ModelComponent();

        std::shared_ptr<Model> model;

        /*Model& operator*() { return *model; }
        const Model& operator*() const { return *model; }
        operator Model() const { return *model; }*/

        //fs::path path;
        //bool castShadows{ false };

        template<typename Archive>
        void serialize(Archive& archive) {
            //archive(cereal::make_nvp("Path", path));
        }
    };
}