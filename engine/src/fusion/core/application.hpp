#pragma once

#include "version.hpp"

namespace fe {
    class Application {
        friend class Engine;
    public:
        Application(std::string name, Version version = {0, 1, 0, 0}) : name{std::move(name)}, version{version} { }
        virtual ~Application() = default;
        NONCOPYABLE(Application);

        /**
         * Run when switching to this app from another.
         */
        virtual void onStart() = 0;

        /**
         * Run every frame as long as the app has work to do.
         */
        virtual void onUpdate() = 0;

        /**
         * Run when ImGui should be drawn.
         */
        virtual void onImGui() = 0;

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
        Version getVersion() const { return version; }

        /**
         * Sets the application's version, for driver support.
         * @param version The application version.
         */
        void setVersion(Version ver) { version = ver; }

    protected:
        std::string name;
        Version version;
        bool started{ false };
    };
}
