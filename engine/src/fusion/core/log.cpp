#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

using namespace fe;

void Log::Init() {
#ifdef FUSION_DEBUG
    static plog::ColorConsoleAppender<plog::TxtFormatter> ConsoleAppender;
    plog::init(plog::debug, &ConsoleAppender);
#else
    const char* file = "fusion.log";
    std::ofstream ofs;
    ofs.open(file, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    plog::init(plog::debug, file);
#endif
}