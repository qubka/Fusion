#pragma once

#include <cereal/cereal.hpp>
#include <uuid.h>

namespace fe {
    struct IdComponent {
        uuids::uuid uuid;

        IdComponent();

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("UUID", uuids::to_string(uuid)));
        }

        template<typename Archive>
        void load(Archive& archive) {
            std::string str;
            archive(cereal::make_nvp("UUID", str));
            uuid = uuids::uuid::from_string(str).value();
        }
    };
}