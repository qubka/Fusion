#pragma once

#include "version.hpp"

namespace fe {
    class Time;
    class App {
        friend class Engine;
    public:
        App(std::string name, const Version& version = {1, 0, 0}) : name{std::move(name)}, version{version} { }
        virtual ~App() = default;
        NONCOPYABLE(App);

        /**
         * Run when switching to this app from another.
         */
        virtual void start() = 0;

        /**
         * Every frame as long as the app has work to do.
         */
        virtual void update(const Time& dt) = 0;

        /**
         * Gets the application's name.
         * @return The application's name.
         */
        const std::string& getName() const { return name; }

        /**
         * Sets the application's name, for driver support.
         * @param name The application's name.
         */
        void setName(const std::string& str) { name = str; }

        /**
         * Gets the application's version.
         * @return The application's version.
         */
        const Version& getVersion() const { return version; }

        /**
         * Sets the application's version, for driver support.
         * @param version The application version.
         */
        void setVersion(const Version& ver) { version = ver; }

    protected:
        std::string name;
        Version version;
        bool started{ false };
    };
}
