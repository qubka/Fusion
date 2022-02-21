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

int main(int argc, char** argv);

namespace Fusion {

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

    public:
        void run();
        // Called if the window is resized and some resources have to be recreatesd
        void recreateSwapchain(const glm::uvec2& newSize);

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
        bool enableVsync{ false };
        // Command buffers used for rendering
        std::vector<vk::CommandBuffer> commandBuffers;
        std::vector<vk::ClearValue> clearValues;
        vk::RenderPassBeginInfo renderPassBeginInfo;
        vk::Viewport viewport() { return vkx::util::viewport(size); }
        vk::Rect2D scissor() { return vkx::util::rect2D(size); }

        virtual void clearCommandBuffers() final;
        virtual void allocateCommandBuffers() final;
        virtual void setupRenderPassBeginInfo();
        virtual void buildCommandBuffers();

    protected:
        // Last frame time, measured using a high performance timer (if available)
        float frameTimer{ 0.0015f };
        // Frame counter to display fps
        uint64_t frameNumber{ 0 };
        uint32_t frameCounter{ 0 };
        uint32_t lastFPS{ 0 };

        // Color buffer format
        vk::Format colorformat{ vk::Format::eB8G8R8A8Unorm };

        // Depth buffer format...  selected during Vulkan initialization
        vk::Format depthFormat{ vk::Format::eUndefined };

        // Global render pass for frame buffer writes
        vk::RenderPass renderPass;

        // List of available frame buffers (same as number of swap chain images)
        std::vector<vk::Framebuffer> framebuffers;
        // Active frame buffer index
        uint32_t currentBuffer = 0;
        // Descriptor set pool
        vk::DescriptorPool descriptorPool;

        void addRenderWaitSemaphore(const vk::Semaphore& semaphore, const vk::PipelineStageFlags& waitStages = vk::PipelineStageFlagBits::eBottomOfPipe);

        std::vector<vk::Semaphore> renderWaitSemaphores;
        std::vector<vk::PipelineStageFlags> renderWaitStages;
        std::vector<vk::Semaphore> renderSignalSemaphores;

        vkx::Context context;
        const vk::PhysicalDevice& physicalDevice{ context.physicalDevice };
        const vk::Device& device{ context.device };
        const vk::Queue& queue{ context.queue };
        const vk::PhysicalDeviceFeatures& deviceFeatures{ context.deviceFeatures };
        vk::PhysicalDeviceFeatures& enabledFeatures{ context.enabledFeatures };
        vkx::ui::UIOverlay ui{ context };

        vk::SurfaceKHR surface;
        // Wraps the swap chain to present images (framebuffers) to the windowing system
        vkx::SwapChain swapChain;

        // Synchronization semaphores
        struct {
            // Swap chain image presentation
            vk::Semaphore acquireComplete;
            // Command buffer submission and execution
            vk::Semaphore renderComplete;
            // UI buffer submission and execution
            vk::Semaphore overlayComplete;
#if 0
            vk::Semaphore transferComplete;
#endif
        } semaphores;

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

        // Command buffer pool
        vk::CommandPool cmdPool;

        uint32_t version = VK_MAKE_VERSION(1, 1, 0);
        vk::Extent2D size{ 1280, 720 };

        std::string title;
        vkx::Image depthStencil;

        CommandLineArgs commandLineArgs;

        LayerStack layers;
        void pushLayer(Layer* layer) { layers.pushFront(layer); }
        void pushOverlay(Layer* layer) { layers.pushBack(layer); }

        // Setup the vulkan instance, enable required extensions and connect to the physical device (GPU)
        virtual void initVulkan();
        virtual void setupSwapchain();
        virtual void setupWindow();

        // A default draw implementation
        virtual void draw();
        // Basic render function
        virtual void render();
        virtual void update(float deltaTime);
        // Prepare commonly used Vulkan functions
        virtual void prepare();

        // Setup default depth and stencil views
        void setupDepthStencil();
        // Create framebuffers for all requested swap chain images
        // Can be overriden in derived class to setup a custom framebuffer (e.g. for MSAA)
        virtual void setupFrameBuffer();

        // Setup a default render pass
        // Can be overriden in derived class to setup a custom render pass (e.g. for MSAA)
        virtual void setupRenderPass();

        void setupUi();

        void updateOverlay();
        virtual void onSetupUIOverlay(vkx::ui::UIOverlayCreateInfo& uiCreateInfo) {}

        virtual void onUpdateCommandBufferPreDraw(const vk::CommandBuffer& commandBuffer) {}
        virtual void onUpdateDrawCommandBuffer(const vk::CommandBuffer& commandBuffer) {}
        virtual void onUpdateCommandBufferPostDraw(const vk::CommandBuffer& commandBuffer) {}

        // Called when view change occurs
        // Can be overriden in derived class to e.g. update uniform buffers
        // Containing view dependant matrices
        virtual void onViewChanged() {}

        // Called when the window has been resized
        // Can be overriden in derived class to recreate or rebuild resources attached to the frame buffer / swapchain
        virtual void onWindowResized() {}

        void drawCurrentCommandBuffer();

        virtual void onLoadAssets() {}

        // Start the main render loop
        void mainLoop();

        // Prepare the frame for workload submission
        // - Acquires the next image from the swap chain
        // - Submits a post present barrier
        // - Sets the default wait and signal semaphores
        void prepareFrame();

        // Submit the frames' workload
        // - Submits the text overlay (if enabled)
        // -
        void submitFrame();

    private:
        // OS specific
#if defined(__ANDROID__)
        // true if application has focused, false if moved to background
        android::Window* window{ nullptr};
        static int32_t handle_input_event(android_app* app, AInputEvent* event);
        static void handle_app_cmd(android_app* app, int32_t cmd);
        void onAppCmd(int32_t cmd);
#else
        glfw::Window* window{ nullptr };
#endif
    };

    Application* CreateApplication(CommandLineArgs args);
}
