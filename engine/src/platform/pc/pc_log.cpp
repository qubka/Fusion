#include "pc_log.h"

#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>

using namespace fe::pc;

namespace plog {
    template<class Formatter>
    class EventConsoleAppender : public ColorConsoleAppender<Formatter> {
    public:
        void write(const Record& record) override{
            ColorConsoleAppender<Formatter>::write(record);// TODO: Create custom formatter
#if FUSION_PLATFORM_WINDOWS
            fe::Log::Get()->onMessage.publish(static_cast<fe::Severity>(record.getSeverity()), fe::String::ConvertUtf8(Formatter::format(record)));
#else
            fe::Log::Get()->onMessage.publish(static_cast<fe::Severity>(record.getSeverity()), Formatter::format(record));
#endif
        }
    };
}

plog::EventConsoleAppender<plog::TxtFormatter> consoleAppender;

Log::Log() : fe::Log{} {
#if FUSION_DEBUG
    plog::init(plog::debug, &consoleAppender);
#else
    const char* file = "fusion.log";
    std::ofstream os;
    os.open(file, std::ofstream::out | std::ofstream::trunc);
    plog::init(plog::debug, file);
#endif
}

Log::~Log() {

}
