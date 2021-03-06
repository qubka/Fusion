#pragma once

#include "version.hpp"

namespace fe {
    class Application {
        friend class Engine;
        friend class ImGuiSubrender;
    public:
        Application(std::string name) : name{std::move(name)} { }
        virtual ~Application() = default;
        NONCOPYABLE(Application);

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

    private:
        std::string name;
        Version version{ 0, 1, 0, 0 };
        bool started{ false };
    };
}
