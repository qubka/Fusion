#include "log.h"

#if FUSION_PLATFORM_ANDROID
#include "platform/android/android_log.h"
#else
#include "platform/pc/pc_log.h"
#endif

using namespace fe;

Log* Log::Instance = nullptr;

Log::Log() {
    Instance = this;
}

Log::~Log() {
    Instance = nullptr;
}

std::unique_ptr<Log> Log::Init() {
    if (Instance != nullptr)
        throw std::runtime_error("Log already instantiated!");

#if FUSION_PLATFORM_ANDROID
    return std::make_unique<android::Log>();
#elif FUSION_PLATFORM_LINUX || FUSION_PLATFORM_WINDOWS || FUSION_PLATFORM_APPLE
    return std::make_unique<pc::Log>();
#else
    FS_LOG_FATAL("Unknown platform!");
    return nullptr;
#endif
}

