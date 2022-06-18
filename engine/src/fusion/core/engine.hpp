#pragma once

#include "version.hpp"
#include "command_line.hpp"
#include "app.hpp"

namespace fe {
    class Engine {
    public:
        /**
         * Gets the engines instance.
         * @return The current engine instance.
         */
        static Engine* Get() { return Instance; }

        /**
         * Carries out the setup for basic engine components and the engine. Call {@link Engine#Run} after creating a instance.
         * @param args The arguments passed to main.
         */
        Engine(const CommandLineArgs& args);
        ~Engine();
        FE_NONCOPYABLE(Engine);

        /**
         * The update function for the updater.
         * @return {@code EXIT_SUCCESS} or {@code EXIT_FAILURE}
         */
        int32_t run();

        /**
         * Gets the command argument passed to main.
         * @return The command parser with parsed data from main.
         */
        const CommandLineParser& getCommandLineParser() const { return commandLineParser; };

        /**
         * Gets the engine's version.
         * @return The engine's version.
         */
        const Version& getVersion() const { return version; }

        /**
         * Gets the current application.
         * @return The renderer manager.
         */
        App* getApp() const { return application.get(); }

        /**
         * Sets the current application to the engine.
         * @param app The new application.
         */
        void setApp(std::unique_ptr<App>&& ptr) { application = std::move(ptr); }

        /**
         * Gets if the engine is running.
         * @return If the engine is running.
         */
        bool isRunning() const { return running; }


    private:
        static Engine* Instance;

        CommandLineParser commandLineParser;
        Version version;

        std::unique_ptr<App> application;

        bool running;
    };
}
