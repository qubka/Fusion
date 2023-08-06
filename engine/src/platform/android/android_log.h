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

#define FS_LOG_TAG "Fusion"

#define FS_LOG(pri, tag, ...) { std::string fmt{ fmt::format(__VA_ARGS__) }; __android_log_print(pri, tag, "%s", fmt.c_str()); fe::Log::Get()->onMessage.publish(fe::android::ConvertPriorityToSeverity<pri>(), fmt); }
#define FS_LOG_VERBOSE(...)   FS_LOG(ANDROID_LOG_VERBOSE, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_DEBUG(...)     FS_LOG(ANDROID_LOG_DEBUG, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_INFO(...)      FS_LOG(ANDROID_LOG_INFO, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_WARNING(...)   FS_LOG(ANDROID_LOG_WARN, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_ERROR(...)     FS_LOG(ANDROID_LOG_ERROR, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_FATAL(...)     FS_LOG(ANDROID_LOG_FATAL, FS_LOG_TAG, __VA_ARGS__)
#define FS_LOG_NONE(...)      FS_LOG(ANDROID_LOG_SILENT, FS_LOG_TAG, __VA_ARGS__)