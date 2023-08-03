#pragma once

#if FUSION_SCRIPTING

namespace fe {
    class ScriptGlue {
    public:
        static void RegisterComponents();
        static void RegisterFunctions();
    };
}

#endif