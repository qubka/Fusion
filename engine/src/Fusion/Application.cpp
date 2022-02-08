#include "Application.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Input/KeyInput.hpp"
#include "Input/MouseInput.hpp"
#include "Layer.hpp"

using namespace Fusion;

static void GLFWErrorCallback(int error, const char* description) {
    FS_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Application::Application() {
    int success = glfwInit();
    FS_CORE_ASSERT(success, "failed to initialize GLFW!");
    glfwSetErrorCallback(GLFWErrorCallback);

    window = Window::create();
    KeyInput::setupKeyInputs(*window);
    MouseInput::setupMouseInputs(*window);
}

Application::~Application() {
    delete window;
    for (auto* input : inputs) {
        delete input;
    }
    for (auto* layer : layers) {
        delete layer;
    }
    glfwTerminate();
}

void Application::run() {
    while (!window->shouldClose()) {
        window->onUpdate();

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        for (auto* input : inputs) {
            input->onUpdate();
        }
    }
}

void Application::pushLayer(Layer& layer) {
    layers.push_front(&layer);
}

void Application::pushOverlay(Layer& overlay) {
    layers.push_back(&overlay);
}

void Application::addInput(Input& input) {
    inputs.push_back(&input);
}
