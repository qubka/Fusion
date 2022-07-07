#pragma once

#include "version.hpp"
#include "command_line.hpp"
#include "module.hpp"
#include "application.hpp"

int main(int argc, char** argv);

namespace fe {
    class DeviceManager;
    class Engine final {
    protected:
        /**
         * Carries out the setup for basic engine components and the engine. Call {@link Engine#Run} after creating a instance.
         * @param args The arguments passed to main.
         */
        Engine(CommandLineArgs args);
        ~Engine();
        NONCOPYABLE(Engine);

        /**
         * The run function for the engine.
         * @return {@code EXIT_SUCCESS} or {@code EXIT_FAILURE}
         */
        int32_t run();

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
        const Version& getVersion() const { return version; }

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
         * Gets if the engine is running.
         * @return If the engine is running.
         */
        bool isRunning() const { return running; }

        /**
         * Requests the engine to stop the game-loop.
         */
        void requestClose() { running = false; }

        /**
         * Gets the engine attached modules.
         * @return The modules map.
         */
        const std::unordered_map<std::type_index, std::unique_ptr<Module>>& getModules() const { return modules; }

    private:
        static Engine* Instance;

        /**
         * The initialization function for the engine.
         */
        void init();

        /**
         * The update engine modules for the required stage.
         */
        void updateStage(Module::Stage stage);

        CommandLineArgs commandLineArgs;
        CommandLineParser commandLineParser;

        Version version;

        std::unique_ptr<Application> application;
        std::unique_ptr<DeviceManager> devices;

        std::unordered_map<std::type_index, std::unique_ptr<Module>> modules;
        std::map<Module::Stage, std::vector<std::type_index>> moduleStages;

        bool running{ false };

        friend int ::main(int argc, char** argv);
    };
}
