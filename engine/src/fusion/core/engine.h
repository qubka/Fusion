#pragma once

#include "version.h"
#include "command_line.h"
#include "application.h"

namespace fe {
    class DeviceManager;
    class ModuleHolder;
    class FUSION_API Engine {
    protected:
        /**
         * Carries out the setup for basic engine components and the engine. Call {@link Engine#Run} after creating a instance.
         * @param args The arguments passed to main.
         */
        explicit Engine(CommandLineArgs&& args);
        virtual ~Engine();
        NONCOPYABLE(Engine)

        /**
         * The run function for the engine.
         * @return {@code EXIT_SUCCESS} or {@code EXIT_FAILURE}
         */
        virtual int32_t run() = 0;

    public:
        /**
         * Gets the engines instance.
         * @return The current engine instance.
         */
        static Engine* Get() { return Instance; }

        /**
         * Gets the command arguments passed to main.
         * @return The command arguments passed tp main.
         */
        const CommandLineArgs& getCommandLineArgs() const { return commandLineArgs; };

        /**
         * Gets the already parsed command argument which was passed to main.
         * @return The command parser with parsed data from main.
         */
        const CommandLineParser& getCommandLineParser() const { return commandLineParser; };

        /**
         * Gets the engine's version.
         * @return The engine's version.
         */
        Version getVersion() const { return version; }

        /**
         * Gets the current application.
         * @return The renderer manager.
         */
        Application* getApp() const { return application.get(); }

        /**
         * Sets the current application to the engine.
         * @param app The new application.
         */
        void setApp(std::unique_ptr<Application>&& app) { application = std::move(app); }

        /**
         * Gets the current app object.
         * @return The object.
         */
        virtual void* getNativeApp() const = 0;

        /**
         * Gets if the engine is running.
         * @return If the engine is running.
         */
        bool isRunning() const { return running; }

        /**
         * Requests the engine to stop the game-loop.
         */
        void requestClose() { running = false; }

    protected:
        /**
         * The initialization function for the engine.
         */
        void init();

        /**
         * The startup function for the application and modules.
         */
        void startup();

        /**
         * The shutdown function for the application and modules.
         */
        void shutdown();

        /**
         * The update main application and devices.
         */
        void updateMain();

        CommandLineArgs commandLineArgs;
        CommandLineParser commandLineParser;

        Version version;

        std::unique_ptr<Log> logger;
        std::unique_ptr<DeviceManager> devices;
        std::unique_ptr<Application> application;
        std::unique_ptr<ModuleHolder> moduleHolder;

        bool running{ false };
        bool started{ false };

        static Engine* Instance;
    };
}
