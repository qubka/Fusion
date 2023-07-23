#pragma once

namespace fe {
    struct IdComponent {
        uint64_t uuid{ Random::get() };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("uuid", uuid));
        }
    };
}