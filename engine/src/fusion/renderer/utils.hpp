#pragma once

#include <string>

namespace vkx {
const std::string& getAssetPath();

enum class LogLevel
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
};

void logMessage(LogLevel level, const char* format, ...);
}  // namespace vkx
