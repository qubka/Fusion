#pragma once

#include <android/log.h>

#include "fusion/core/log.h"

namespace fe::android {
    class FUSION_API Log : public fe::Log {
    public:
        Log() = default;
        ~Log() override = default;
    };

    template<int P>
    constexpr fe::Severity ConvertPriorityToSeverity() {
        if constexpr (P == ANDROID_LOG_VERBOSE)
            return fe::Severity::Verbose;
        else if constexpr (P == ANDROID_LOG_DEBUG)
            return fe::Severity::Debug;
        else if constexpr (P == ANDROID_LOG_INFO)
            return fe::Severity::Info;
        else if constexpr (P == ANDROID_LOG_WARN)
            return fe::Severity::Warning;
        else if constexpr (P == ANDROID_LOG_ERROR)
            return fe::Severity::Error;
        else if constexpr (P == ANDROID_LOG_FATAL)
            return fe::Severity::Fatal;
        else
            return fe::Severity::None;
    }
}

#define FE_LOG_TAG "Fusion"

#define FE_LOG(pri, tag, ...) { std::string fmt{ fmt::format(__VA_ARGS__) }; __android_log_print(pri, tag, "%s", fmt.c_str()); fe::Log::Get()->onMessage.publish(fe::android::ConvertPriorityToSeverity<pri>(), fmt); }
#define FE_LOG_VERBOSE(...)   FE_LOG(ANDROID_LOG_VERBOSE, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_DEBUG(...)     FE_LOG(ANDROID_LOG_DEBUG, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_INFO(...)      FE_LOG(ANDROID_LOG_INFO, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_WARNING(...)   FE_LOG(ANDROID_LOG_WARN, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_ERROR(...)     FE_LOG(ANDROID_LOG_ERROR, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_FATAL(...)     FE_LOG(ANDROID_LOG_FATAL, FE_LOG_TAG, __VA_ARGS__)
#define FE_LOG_NONE(...)      FE_LOG(ANDROID_LOG_SILENT, FE_LOG_TAG, __VA_ARGS__)