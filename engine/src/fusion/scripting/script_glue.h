#pragma once

#if FUSION_SCRIPTING

namespace fe {
    class FUSION_API ScriptGlue {
    public:
        static void RegisterComponents();
        static void RegisterFunctions();
    };
}

#endif