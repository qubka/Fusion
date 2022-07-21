#include "log.hpp"

#include <plog/Initializers/RollingFileInitializer.h>

using namespace fe;

plog::EventConsoleAppender<plog::TxtFormatter> Log::ConsoleAppender;

void Log::Init() {
#if FUSION_DEBUG
    plog::init(plog::debug, &ConsoleAppender);
#else
    const char* file = "fusion.log";
    std::ofstream os;
    os.open(file, std::ofstream::out | std::ofstream::trunc);
    plog::init(plog::debug, file);
#endif
}