#pragma once

#include "version.hpp"
#include "layer_stack.hpp"

#include "fusion/renderer/graphics.hpp"

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
        virtual void onStart() = 0;

        /**
         * Every frame as long as the app has work to do.
         */
        virtual void onUpdate(const Time& dt) = 0;

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

        /**
         * Gets the current graphics context.
         * @return The graphics context.
         */
        Graphics* getGraphics() { return graphics.get(); }

        /**
         * Sets the current graphics context to the application.
         * @param graphics The new graphics context.
         */
        void setGraphics(std::unique_ptr<Graphics>&& graphics) { this->graphics = std::move(graphics); }

        void pushLayer(Layer* layer) { layers.pushFront(layer); }

        void pushOverlay(Layer* layer) { layers.pushBack(layer); }

    protected:
        std::string name;
        Version version;
        bool started{ false };

        std::unique_ptr<fe::Graphics> graphics;

        LayerStack layers;

        //KeyInput keyInput{/*all keys*/};
        //MouseInput mouseInput{/*all buttons*/};
    };
}
