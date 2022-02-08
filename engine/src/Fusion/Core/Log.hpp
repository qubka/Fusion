#pragma once

#include "Base.hpp"
#include "spdlog/spdlog.h"

namespace Fusion {
    class FUSION_API Log {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; }
        static std::shared_ptr<spdlog::logger>& GetGameLogger() { return gameLogger; }
    private:
        static std::shared_ptr<spdlog::logger> coreLogger;
        static std::shared_ptr<spdlog::logger> gameLogger;
    };
}

#define FS_LOG_CORE_TRACE(...) ::Fusion::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FS_LOG_CORE_INFO(...) ::Fusion::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FS_LOG_CORE_WARN(...) ::Fusion::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FS_LOG_CORE_ERROR(...) ::Fusion::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FS_LOG_CORE_FATAL(...) ::Fusion::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define FS_LOG_TRACE(...) ::Fusion::Log::GetGameLogger()->trace(__VA_ARGS__)
#define FS_LOG_INFO(...) ::Fusion::Log::GetGameLogger()->info(__VA_ARGS__)
#define FS_LOG_WARN(...) ::Fusion::Log::GetGameLogger()->warn(__VA_ARGS__)
#define FS_LOG_ERROR(...) ::Fusion::Log::GetGameLogger()->error(__VA_ARGS__)
#define FS_LOG_FATAL(...) ::Fusion::Log::GetGameLogger()->critical(__VA_ARGS__)