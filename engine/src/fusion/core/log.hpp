#pragma once

#include <plog/Log.h>

namespace fe {
    // Define log instances. Default is 0 and is omitted from this enum
    /*enum {
        EngineLog = 0,
        GameLog = 1
    };*/

    class Log {
    public:
        static void Init();
    };
}