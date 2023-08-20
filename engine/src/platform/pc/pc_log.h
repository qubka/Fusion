#pragma once

#include <plog/Log.h>

#include "fusion/core/log.h"

namespace fe::pc {
    class FUSION_API Log final : public fe::Log {
    public:
        Log();
        ~Log() override;
    };
}

#define FE_LOG_VERBOSE(...)   LOG_VERBOSE << fmt::format(__VA_ARGS__)
#define FE_LOG_DEBUG(...)     LOG_DEBUG << fmt::format(__VA_ARGS__)
#define FE_LOG_INFO(...)      LOG_INFO << fmt::format(__VA_ARGS__)
#define FE_LOG_WARNING(...)   LOG_WARNING << fmt::format(__VA_ARGS__)
#define FE_LOG_ERROR(...)     LOG_ERROR << fmt::format(__VA_ARGS__)
#define FE_LOG_FATAL(...)     LOG_FATAL << fmt::format(__VA_ARGS__)
#define FE_LOG_NONE(...)      LOG_NONE << fmt::format(__VA_ARGS__)