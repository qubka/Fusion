#include "Application.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Input.hpp"
#include "Layer.hpp"
#include "Events/Event.hpp"
#include "Events/ApplicationEvents.hpp"

using namespace Fusion;

static void GLFWErrorCallback(int error, const char* description) {
    FS_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Application::Application() {
    int success = glfwInit();
    FS_CORE_ASSERT(success, "failed to initialize GLFW!");
    glfwSetErrorCallback(GLFWErrorCallback);

    window = Window::create();
    window->setEventCallback(BIND_EVENT_FUNC(onEvent));

    //input = Input::create(*window);
}

Application::~Application() {
    //delete input;
    delete window;
    for (auto* layer : layers) {
        delete layer;
    }
    glfwTerminate();
}

void Application::run() {
    while (isRunning) {
        window->onUpdate();

        for (auto* layer : layers) {
            layer->onUpdate();
        }
    }
}

void Application::pushLayer(Layer& layer) {
    layers.push_front(&layer);
}

void Application::pushOverlay(Layer& overlay) {
    layers.push_back(&overlay);
}

void Application::onEvent(Event& event) {
    FS_LOG_INFO("{0}", event.toString());

    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(onWindowClose));

    for (auto it = layers.end(); it != layers.begin(); ) {
        (*--it)->onEvent(event);
        if (event.isHandled())
            break;
    }
}

bool Application::onWindowClose(WindowCloseEvent& event) {
    isRunning = false;
    return true;
}