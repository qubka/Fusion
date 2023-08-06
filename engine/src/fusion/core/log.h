#pragma once

namespace fe {
    enum class Severity : uint8_t {
        None = 0,
        Fatal = 1,
        Error = 2,
        Warning = 3,
        Info = 4,
        Debug = 5,
        Verbose = 6
    };

    class FUSION_API Log {
    public:
        Log();
        virtual ~Log();
        NONCOPYABLE(Log);

        static std::unique_ptr<Log> Init();

        static Log* Get() { return Instance; }

        /**
         * Event when the message is written to console.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMessage() { return entt::sink{onMessage}; }

        // Unprotected
        entt::sigh<void(Severity, const std::string&)> onMessage;

    protected:
        static Log* Instance;
    };
}