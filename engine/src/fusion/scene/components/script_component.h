#pragma once

#include "fusion/scripting/script_engine.h"

namespace fe {
    struct ScriptComponent {
#if FUSION_SCRIPTING
        std::shared_ptr<ScriptInstance> instance;
        std::string className;
        ScriptFieldMap fields;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("className", className));
            archive(cereal::make_nvp("fields", fields));
        }
#endif
    };
}