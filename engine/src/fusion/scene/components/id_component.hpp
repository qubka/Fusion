#pragma once

namespace fe {
    struct IdComponent {
        uuids::uuid uuid{ uuid_random_generator() };

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("uuid", uuids::to_string(uuid)));
        }

        template<typename Archive>
        void load(Archive& archive) {
            std::string str;
            archive(cereal::make_nvp("uuid", str));
            uuid = uuids::uuid::from_string(str).value_or(uuid_random_generator());
        }
    };
}