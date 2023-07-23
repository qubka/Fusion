#pragma once

namespace fe {
    struct NameComponent {
        std::string name;

        std::string& operator*() { return name; }
        const std::string& operator*() const { return name; }
        operator std::string() const { return name; }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("name", name));
        }
    };
}
