#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/include/spdlog/sinks/basic_file_sink.h"

namespace Fusion {
    std::shared_ptr<spdlog::logger> Log::coreLogger;
    std::shared_ptr<spdlog::logger> Log::gameLogger;

    void Log::Init() {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("fusion.log", true));

        sinks[0]->set_pattern("%^[%T] %n: %v%$");
        sinks[1]->set_pattern("[%T] [%l] %n: %v");

        coreLogger = std::make_shared<spdlog::logger>("CORE", begin(sinks), end(sinks));
        spdlog::register_logger(coreLogger);
        coreLogger->set_level(spdlog::level::trace);
        coreLogger->flush_on(spdlog::level::trace);

        coreLogger = std::make_shared<spdlog::logger>("GAME", begin(sinks), end(sinks));
        spdlog::register_logger(coreLogger);
        coreLogger->set_level(spdlog::level::trace);
        coreLogger->flush_on(spdlog::level::trace);
    }
}