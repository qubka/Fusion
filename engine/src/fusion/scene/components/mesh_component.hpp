#pragma once

//#include "fusion/mesh/model.hpp"

namespace fe {
    class Mesh;
    struct MeshComponent {
        std::shared_ptr<Mesh> runtime;

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