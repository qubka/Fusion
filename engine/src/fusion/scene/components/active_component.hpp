#pragma once

#include <cereal/cereal.hpp>

namespace fe {
    struct ActiveComponent {
        bool active{ true };

        bool& operator*() { return active; }
        const bool& operator*() const { return active; }
        operator bool() const { return active; }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Active", active));
        }
    };
}