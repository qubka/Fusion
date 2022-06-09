#pragma once

#include "layer_stack.hpp"
#include "command_line.hpp"

#include "fusion/renderer/vkx/helpers.hpp"
#include "fusion/renderer/vkx/filesystem.hpp"
#include "fusion/renderer/vkx/model.hpp"
#include "fusion/renderer/vkx/shaders.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/vkx/texture.hpp"
#include "fusion/renderer/vkx/benchmark.hpp"
#include "fusion/renderer/renderer.hpp"
#include "fusion/utils/random.hpp"

#if defined(__ANDROID__)
#include "fusion/renderer/android/window.hpp"
#else
#include "fusion/renderer/glfw/window.hpp"
#endif

int main(int argc, char** argv);

namespace fe {
    class Application {
    protected:
        Application(std::string title = "Fusion", CommandLineArgs args = {});
        virtual ~Application();
        FE_NONCOPYABLE(Application);

        void run();

    public:
        static Application& Instance() { assert(instance && "Application was not initialized!"); return *instance; }
        Window* getWindow() { return window; }
        Renderer& getRenderer() { return renderer; }
        KeyInput& getKeyInput() { return keyInput; }
        MouseInput& getMouseInput() { return mouseInput; }

    private:
        // fps timer (one second interval)
        float fpsTimer{ 0.0f };
        // Get window title with example name, device, et.
        std::string getWindowTitle();
        std::string title;
        // Default window size
        vk::Extent2D size{ 3840, 2160 };

        // Used to store app singleton
        static Application* instance;
        friend int ::main(int argc, char** argv);

    protected:
        /** @brief Example settings that can be changed e.g. by command line arguments */
        struct Settings {
            /** @brief Activates validation layers (and message output) when set to true */
            bool validation{ false };
            /** @brief Set to true if fullscreen mode has been requested via command line */
            bool fullscreen{ false };
            /** @brief Set to true if v-sync will be forced for the swapchain */
            bool vsync{ false };
            /** @brief Enable UI overlay */
            bool overlay{ true };
        } settings;

        vkx::Benchmark benchmark;

        // Frame counter to display fps
        uint64_t frameNumber{ 0 };
        uint32_t frameCounter{ 0 };
        uint32_t lastFPS{ 0 };

        vkx::Context context;
        const vk::PhysicalDevice& physicalDevice{ context.physicalDevice };
        const vk::Device& device{ context.device };
        const vk::Queue& queue{ context.queue };
        const vk::PhysicalDeviceFeatures& deviceFeatures{ context.deviceFeatures };
        vk::PhysicalDeviceFeatures& enabledFeatures{ context.enabledFeatures };
        fe::Renderer renderer{ context };

    protected:
        uint32_t version = VK_MAKE_VERSION(1, 1, 0);
        CommandLineParser commandLineParser;

        LayerStack layers;
        void pushLayer(Layer* layer) { layers.pushFront(layer); }
        void pushOverlay(Layer* layer) { layers.pushBack(layer); }

        void mainInit();
        void mainLoop();

        // Setup the vulkan instance, enable required extensions and connect to the physical device (GPU)
        void setupVulkan();
        void setupRenderer();
        void setupWindow();

        virtual void render();
        virtual void update(float dt);

    private:
        // OS specific
#if defined(__ANDROID__)
        static int32_t handle_input_event(android_app* app, AInputEvent* event);
        static void handle_app_cmd(android_app* app, int32_t cmd);
        void onAppCmd(int32_t cmd);
#elif defined(_WIN32)
        void setupConsole(std::string title);
#endif
        Window* window{ nullptr };
        KeyInput keyInput{/*all keys*/};
        MouseInput mouseInput{/*all buttons*/};
    };

    Application* CreateApplication(CommandLineArgs args);
}
