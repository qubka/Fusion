#pragma once

#include <plog/Log.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include "fusion/utils/string.h"

namespace plog {
    template<class Formatter>
    class EventConsoleAppender : public ColorConsoleAppender<Formatter> {
    public:
        void write(const Record& record) override{
            ColorConsoleAppender<Formatter>::write(record);// TODO: Create custom formatter
#if FUSION_PLATFORM_WINDOWS
            onMessage.publish(record, fe::String::ConvertUtf8(Formatter::format(record)));
#else
            onMessage.publish(record, Formatter::format(record));
#endif
        }

        /**
         * Event when the message is written to console.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMessage() { return entt::sink{onMessage}; }

    private:
        entt::sigh<void(const Record&, const std::string&)> onMessage;
    };
}

namespace fe {
    class Log {
    public:
        static void Init();

        static plog::EventConsoleAppender<plog::TxtFormatter>& GetConsoleAppender() { return ConsoleAppender; }

    private:
        static plog::EventConsoleAppender<plog::TxtFormatter> ConsoleAppender;
    };
}
