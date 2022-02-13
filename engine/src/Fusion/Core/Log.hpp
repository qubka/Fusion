#pragma once

#include <plog/Log.h>

namespace Fusion {
    // Define log instances. Default is 0 and is omitted from this enum.
    enum {
        EngineLog = 0,
        GameLog = 1
    };

    class FUSION_API Log {
    public:
        static void Init();
    };
}

#define FE_CORE_DEBUG PLOG_DEBUG
#define FE_CORE_INFO  PLOG_INFO
#define FE_CORE_WARNING PLOG_WARNING
#define FE_CORE_ERROR PLOG_ERROR
#define FE_CORE_FATAL PLOG_FATAL

#define FE_LOG_DEBUG PLOGD_(Fusion::GameLog, plog::debug)
#define FE_LOG_INFO PLOG_(Fusion::GameLog, plog::info)
#define FE_LOG_WARNING PLOGD_(Fusion::GameLog, plog::warning)
#define FE_LOG_ERROR PLOGD_(Fusion::GameLog, plog::error)
#define FE_LOG_FATAL PLOGD_(Fusion::GameLog, plog::fatal)