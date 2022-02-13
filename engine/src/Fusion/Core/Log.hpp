#pragma once

#include <plog/Log.h>

namespace Fusion {
    // Define log instances. Default is 0 and is omitted from this enum.
    /*enum {
        EngineLog = 0,
        GameLog = 1
    };*/

    class FUSION_API Log {
    public:
        static void Init();
    };
}

#define FE_LOG_DEBUG PLOG_DEBUG
#define FE_LOG_INFO  PLOG_INFO
#define FE_LOG_WARNING PLOG_WARNING
#define FE__LOG_ERROR PLOG_ERROR
#define FE_LOG_FATAL PLOG_FATAL