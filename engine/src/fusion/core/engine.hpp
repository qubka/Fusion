#pragma once

#include "version.hpp"
#include "command_line.hpp"
#include "app.hpp"

#include "fusion/utils/time.hpp"
#include "fusion/devices/device_manager.hpp"

int main(int argc, char** argv);

namespace fe {
    class Engine {
    protected:
        /**
         * Carries out the setup for basic engine components and the engine. Call {@link Engine#Run} after creating a instance.
         * @param args The arguments passed to main.
         */
        Engine(const CommandLineArgs& args);
        ~Engine();
        NONCOPYABLE(Engine);

        /**
         * The update function for the updater.
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
        void setApp(std::unique_ptr<App>&& app) { application = std::move(app); }

        /**
         * Gets if the engine is running.
         * @return If the engine is running.
         */
        bool isRunning() const { return running; }

        /**
         * This value starts at 0 and increases by 1 on each run phase.
         * @return The total number of frames since the start of the game.
         */
        uint64_t frameCount() const { return frameNumber; }

        /**
         * Requests the engine to stop the game-loop.
         */
        void requestClose() { running = false; }

    private:
        struct DeltaTime {
            Time currentTime;
            Time lastTime;
            Time time;

            void update() {
                currentTime = Time::Now();
                time = currentTime - lastTime;
                lastTime = currentTime;
            }
        } deltaTime;

        /*struct ChangePerSecond {
            uint32_t value{ 0 };
            uint32_t tempValue{ 0 };
            Time valueTime;

            void update(const Time &time) {
                tempValue++;

                if (std::floor(time.asSeconds()) > std::floor(valueTime.asSeconds())) {
                    value = tempValue;
                    tempValue = 0;
                }

                valueTime = time;
            }
        } fps;*/

        static Engine* Instance;

        CommandLineParser commandLineParser;
        Version version;

        std::unique_ptr<Devices> devices;
        std::unique_ptr<App> application;

        uint64_t frameNumber{ 0 };
        bool running{ false };

        friend int ::main(int argc, char** argv);
    };
}
