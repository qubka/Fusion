#pragma once

#include "fusion/renderer/glfw/window.hpp"
#include "layer_stack.hpp"

#include "fusion/renderer/vkx/helpers.hpp"
#include "fusion/renderer/vkx/filesystem.hpp"
#include "fusion/renderer/vkx/model.hpp"
#include "fusion/renderer/vkx/shaders.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/vkx/texture.hpp"
#include "fusion/renderer/vkx/swapchain.hpp"
#include "fusion/renderer/vkx/ui.hpp"
#include "fusion/renderer/renderer.hpp"

int main(int argc, char** argv);

namespace fe {
    struct CommandLineArgs {
        int count{ 0 };
        char** args{ nullptr };

        const char* operator[](int index) const {
            assert(index < count && "Invalid argument index");
            return args[index];
        }
    };

    struct UpdateOperation {
        const vk::Buffer buffer;
        const vk::DeviceSize size;
        const vk::DeviceSize offset;
        const uint32_t* data;

        template <typename T>
        UpdateOperation(const vk::Buffer& buffer, const T& data, vk::DeviceSize offset = 0)
                : buffer{buffer}
                , size{sizeof(T)}
                , offset{offset}
                , data{reinterpret_cast<uint32_t*>(&data)} {
            assert(0 == (sizeof(T) % 4));
            assert(0 == (offset % 4));
        }
    };

    class Application {
    protected:
        Application(std::string title = "Fusion", CommandLineArgs args = {});
        virtual ~Application();
        FE_NONCOPYABLE(Application);

        void run();

    public:
        static const Application& Instance() { return *instance; }
        const fe::KeyInput& getKeyInput() const { return window->getKeyInput(); }
        const fe::MouseInput& getMouseInput() const { return window->getMouseInput(); }
        fe::Window& getWindow() const { return *window; }

    private:
        // Set to true when the debug marker extension is detected
        bool enableDebugMarkers{ false };
        // fps timer (one second interval)
        float fpsTimer { 0.0f };
        // Get window title with example name, device, et.
        std::string getWindowTitle();
        // Used to store app singleton
        static Application* instance;
        friend int ::main(int argc, char** argv);

    protected:
        /** @brief Example settings that can be changed e.g. by command line arguments */
        struct Settings {
            /** @brief Activates validation layers (and message output) when set to true */
            bool validation { false };
            /** @brief Set to true if fullscreen mode has been requested via command line */
            bool fullscreen { false };
            /** @brief Set to true if v-sync will be forced for the swapchain */
            bool vsync { false };
            /** @brief Enable UI overlay */
            bool overlay { true };
        } settings;

        // Last frame time, measured using a high performance timer (if available)
        float frameTimer{ 0.0015f };
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
        vkx::ui::UIOverlay ui{ context };
        fe::Renderer renderer{ context };

    protected:
        uint32_t version = VK_MAKE_VERSION(1, 1, 0);
        vk::Extent2D size{ 1280, 720 };

        std::string title;

        CommandLineArgs commandLineArgs;

        LayerStack layers;
        void pushLayer(Layer* layer) { layers.pushFront(layer); }
        void pushOverlay(Layer* layer) { layers.pushBack(layer); }

        void mainInit();
        void mainLoop();

        // Setup the vulkan instance, enable required extensions and connect to the physical device (GPU)
        void initVulkan();
        void setupRenderer();
        void setupWindow();
        void setupUi();

        virtual void render();
        virtual void update(float deltaTime);

        void updateOverlay();
        void drawUI(const vk::CommandBuffer& commandBuffer);

    private:
        // OS specific
#if defined(__ANDROID__)
        static int32_t handle_input_event(android_app* app, AInputEvent* event);
        static void handle_app_cmd(android_app* app, int32_t cmd);
        void onAppCmd(int32_t cmd);
#endif
        Window* window{ nullptr };
    };

    Application* CreateApplication(CommandLineArgs args);
}
