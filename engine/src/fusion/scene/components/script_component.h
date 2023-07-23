#pragma once

namespace fe {
    struct ScriptComponent {
        std::string className;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("className", className));
        }
    };
}