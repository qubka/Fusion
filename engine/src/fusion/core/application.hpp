#pragma once

#include "version.hpp"

namespace fe {
    class Application {
        friend class Engine;
        friend class ImGuiSubrender;
    public:
        explicit Application(std::string_view name) : name{name} { }
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
        void setName(std::string_view str) { name = str; }

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

        /**
         *
         * @return
         */
        virtual const fs::path& getRootPath() const = 0;

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

        /**
         * Called when app is being swapped and will no longer be rendered/updated.
         */
        virtual void onStop() = 0;

    private:
        std::string name;
        Version version{ 0, 1, 0, 0 };
        bool started{ false };
    };
}
