#include "window.hpp"

#include "fusion/core/time.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/devices/cursor.hpp"

using namespace glfw;

Window::Window(const fe::VideoMode& videoMode, const glm::uvec2& size, std::string label) : fe::Window{}
    , size{size}
    , title{std::move(label)}
    , resizable{true}
    , visible{true}
    , focused{true}
{
    LOG_INFO << "Creating window: " << title << " [" << size.x << " " << size.y << "]";

    // Create a windowed mode window and its context.
    window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

    // Gets any window errors.
    if (!window) {
        throw std::runtime_error("GLFW failed to create the window!");
    }

    // Sets the user pointer.
    glfwSetWindowUserPointer(window, this);

    // Window attributes that can change later.
    glfwSetWindowAttrib(window, GLFW_DECORATED, !borderless);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
    glfwSetWindowAttrib(window, GLFW_FLOATING, floating);

    // Centre the window position.
    position.x = (videoMode.width - size.x) / 2;
    position.y = (videoMode.height - size.y) / 2;
    glfwSetWindowPos(window, position.x, position.y);

    // Sets fullscreen if enabled.
    if (fullscreen)
        setFullscreen(true);

    // Sets the window callbacks.
    glfwSetWindowPosCallback(window, PosCallback);
    glfwSetWindowSizeCallback(window, SizeCallback);
    glfwSetWindowCloseCallback(window, CloseCallback);
    glfwSetWindowRefreshCallback(window, RefreshCallback);
    glfwSetWindowFocusCallback(window, FocusCallback);
    glfwSetWindowIconifyCallback(window, IconifyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    // Sets the inputs callbacks.
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
#if GLFW_VERSION_MINOR >= 1
    glfwSetDropCallback(window, FileDropCallback);
#endif
#if GLFW_VERSION_MINOR >= 3
    glfwSetWindowMaximizeCallback(window, MaximizeCallback);
    glfwSetWindowContentScaleCallback(window, ContentScaleCallback);
#endif

    // Shows the glfw window.
    if (visible)
        glfwShowWindow(window);

    onStart.publish();
}

Window::~Window() {
    glfwDestroyWindow(window);
}

void Window::update() {
    float dt = fe::Time::DeltaTime();

    // Updates the position delta.
    mousePositionDelta = dt * (mouseLastPosition - mousePosition);
    mouseLastPosition = mousePosition;

    // Updates the scroll delta.
    mouseScrollDelta = dt * (mouseLastScroll - mouseScroll);
    mouseLastScroll = mouseScroll;
}

void Window::setBorderless(bool flag) {
    borderless = flag;
    glfwSetWindowAttrib(window, GLFW_DECORATED, !borderless);
    onBorderless.publish(flag);
}

void Window::setResizable(bool flag) {
    resizable = flag;
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
    onResizable.publish(flag);
}

void Window::setFloating(bool flag) {
    floating = flag;
    glfwSetWindowAttrib(window, GLFW_FLOATING, floating);
    onFloating.publish(flag);
}

void Window::setFullscreen(bool flag, fe::Monitor* monitor) {
    auto selected = monitor ? monitor : getCurrentMonitor();
    auto videoMode = selected->getVideoMode();

    fullscreen = flag;

    if (fullscreen) {
        LOG_DEBUG << "Window is going fullscreen";
        fullscreenSize = {videoMode.width, videoMode.height};
        glfwSetWindowMonitor(window, reinterpret_cast<GLFWmonitor*>(selected->getNativeMonitor()), 0, 0, fullscreenSize.x, fullscreenSize.y, GLFW_DONT_CARE);
    } else {
        LOG_DEBUG << "Window is going windowed";
        position = ((glm::uvec2{videoMode.width, videoMode.height} - size) / 2U) + selected->getPosition();
        glfwSetWindowMonitor(window, nullptr, position.x, position.y, size.x, size.y, GLFW_DONT_CARE);
    }

    onFullscreen.publish(fullscreen);
}

void Window::setFocused() {
    if (!focused && visible && !iconified) {
        glfwFocusWindow(window);
    }
}
void Window::setIconified(bool flag) {
    if (!iconified && flag) {
        glfwIconifyWindow(window);
    } else if (iconified && !flag) {
        glfwRestoreWindow(window);
    }
}

void Window::setVisible(bool flag) {
    if (!visible && flag) {
        glfwShowWindow(window);
    } else if (visible && !flag){
        glfwHideWindow(window);
    }
    visible = flag;
    onShow.publish(flag);
}

void Window::setSize(const glm::ivec2& extent) {
    if (extent.x != -1)
        size.x = extent.x;
    if (extent.y != -1)
        size.y = extent.y;
    glfwSetWindowSize(window, size.x, size.y);
}
void Window::setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize) {
    glfwSetWindowSizeLimits(window, minSize.x, minSize.y, (maxSize.x != 0) ? maxSize.x : minSize.x, (maxSize.y != 0) ? maxSize.y : minSize.y);
}

void Window::setPosition(const glm::ivec2& pos) {
    if (pos.x != -1)
        position.x = pos.x;
    if (pos.x != -1)
        position.y = pos.y;
    glfwSetWindowPos(window, position.x, position.y);
}

void Window::setTitle(const std::string& str) {
    title = str;
    glfwSetWindowTitle(window, title.c_str());
    onTitleChange.publish(str);
}

void Window::setCursorHidden(bool hidden) {
    if (cursorHidden != hidden) {
        glfwSetInputMode(window, GLFW_CURSOR, hidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        if (!hidden && cursorHidden)
            setMousePosition(mousePosition);

        cursorHidden = hidden;
    }
}

void Window::setCursor(const fe::Cursor* cursor) {
    glfwSetCursor(window, cursor ? reinterpret_cast<GLFWcursor*>(cursor->getNativeCursor()) : nullptr);
}

void Window::setIcons(const std::vector<std::filesystem::path>& filenames) {
    std::vector<std::unique_ptr<fe::Bitmap>> bitmaps;
    bitmaps.reserve(filenames.size());

    std::vector<GLFWimage> icons;
    icons.reserve(filenames.size());

    for (const auto& filename : filenames) {
        auto bitmap = std::make_unique<fe::Bitmap>(filename);
        if (!bitmap)
            continue;

        GLFWimage icon = {};
        icon.width = bitmap->getWidth();
        icon.height = bitmap->getHeight();
        icon.pixels = bitmap->getData<uint8_t>();
        icons.push_back(icon);
        bitmaps.push_back(std::move(bitmap));
    }

    glfwSetWindowIcon(window, static_cast<int>(icons.size()), icons.data());
}

fe::ActionCode Window::getKey(fe::KeyCode key) const {
    return glfwGetKey(window, key);
}

fe::ActionCode Window::getMouseButton(fe::MouseButton button) const {
    return glfwGetMouseButton(window, button);
}

void Window::setMousePosition(const glm::vec2& pos) {
    mouseLastPosition = pos;
    mousePosition = pos;
    glfwSetCursorPos(window, pos.x, pos.y);
}

void Window::setMouseScroll(const glm::vec2& scroll) {
    mouseLastScroll = scroll;
    mouseScroll = scroll;
}

VkResult Window::createSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const {
    return glfwCreateWindowSurface(instance, window, allocator, surface);
}

int overlappingArea(const glm::ivec2& l1, const glm::ivec2& r1, const glm::ivec2& l2, const glm::ivec2& r2) {
    int area1 = std::abs(l1.x - r1.x) * std::abs(l1.y - r1.y);

    int area2 = std::abs(l2.x - r2.x) * std::abs(l2.y - r2.y);

    int areaI = (std::min(r1.x, r2.x) - std::max(l1.x, l2.x)) * (std::min(r1.y, r2.y) - std::max(l1.y, l2.y));

    return area1 + area2 - areaI;
}

const fe::Monitor* Window::getCurrentMonitor() const {
    if (fullscreen) {
        auto glfwMonitor = glfwGetWindowMonitor(window);
        for (const auto& monitor : fe::Devices::Get()->getMonitors()) {
            if (monitor->getNativeMonitor() == glfwMonitor)
                return monitor.get();
        }
        return nullptr;
    }

    std::multimap<int, const fe::Monitor*> rankedMonitor;
    auto where = rankedMonitor.end();

    for (const auto& monitor : fe::Devices::Get()->getMonitors()) {
        where = rankedMonitor.insert(where, { overlappingArea(monitor->getWorkareaPosition(), monitor->getWorkareaPosition() + monitor->getWorkareaSize(), position, position + size), monitor.get() });
    }

    if (rankedMonitor.begin()->first > 0)
        return rankedMonitor.begin()->second;

    return nullptr;
}

/* Events Callbacks */

namespace glfw {

    void Window::PosCallback(GLFWwindow* handle, int x, int y) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::uvec2 pos {x, y};

        LOG_VERBOSE << "PositionChangeEvent: " << glm::to_string(pos);

        window.position = pos;
        window.onPositionChange.publish(pos);
    }

    void Window::SizeCallback(GLFWwindow* handle, int width, int height) {
        if (width <= 0 || height <= 0)
            return;

        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::uvec2 size {width, height};

        LOG_VERBOSE << "SizeChangeEvent: " << glm::to_string(size);

        if (window.fullscreen) {
            window.fullscreenSize = size;
        } else {
            window.size = size;
        }
        window.onSizeChange.publish(size);
    }

    void Window::CloseCallback(GLFWwindow* handle) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "CloseEvent";

        window.onClose.publish();
    }

    void Window::RefreshCallback(GLFWwindow* handle) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "RefreshEvent";

        window.onRefresh.publish();
    }

    void Window::FocusCallback(GLFWwindow* handle, int focused) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "FocusEvent: " << (focused ? "TRUE" : "FALSE");

        window.focused = focused;
        window.onFocus.publish(focused);
    }

    void Window::IconifyCallback(GLFWwindow* handle, int iconified) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "IconifyEvent: " << (iconified ? "TRUE" : "FALSE");

        window.iconified = iconified;
        window.onIconify.publish(iconified);
    }

    void Window::FramebufferSizeCallback(GLFWwindow* handle, int width, int height) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::uvec2 size {width, height};

        LOG_VERBOSE << "FramebufferResize: " << glm::to_string(size);

        if (window.fullscreen) {
            window.fullscreenSize = size;
        } else {
            window.size = size;
        }
        window.onFramebufferResize.publish(size);
    }

    void Window::CursorPosCallback(GLFWwindow* handle, double posX, double posY) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::vec2 pos {posX, posY};

        LOG_VERBOSE << "MouseMotionEvent: " << glm::to_string(pos);

        window.mousePosition = pos;
        window.onMouseMotion.publish(pos);

        const auto& size = window.getSize();
        if (size.x <= 1 || size.y <= 1) {
            return;
        }

        glm::vec2 norm{ 2.0f * pos / glm::vec2{size - 1U}};

        LOG_VERBOSE << "MouseMotionNormEvent: " << glm::to_string(norm);

        window.mousePositionNorm = norm;
        window.onMouseMotionNorm.publish(norm);
    }

    void Window::ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::vec2 offset {offsetX, offsetY};

        LOG_VERBOSE << "MouseScrollEvent: " << glm::to_string(offset);

        window.mouseScroll = offset;
        window.onMouseScroll.publish(offset);
    }

    void Window::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "MouseButtonEvent: " << glm::to_string(glm::ivec3{button, action, mods});

        window.onMouseButton.publish(fe::MouseData{ static_cast<fe::MouseButton>(button), static_cast<fe::ActionCode>(action), static_cast<fe::ModCode>(mods) });

        switch (action) {
            case GLFW_PRESS:
                window.onMousePress.publish(button);
                break;
            case GLFW_RELEASE:
                window.onMouseRelease.publish(button);
                break;
        }
    }

    void Window::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "KeyEvent: " << glm::to_string(glm::ivec4{key, scancode, action, mods});

        window.onKey.publish(fe::KeyData{static_cast<fe::KeyCode>(key), static_cast<fe::ActionCode>(action), static_cast<fe::ScanCode>(scancode), static_cast<fe::ModCode>(mods)});

        switch (action) {
            case GLFW_PRESS:
                window.onKeyPress.publish(key);
                break;
            case GLFW_RELEASE:
                window.onKeyRelease.publish(key);
                break;
            case GLFW_REPEAT:
                window.onKeyHold.publish(key);
                break;
        }
    }

    void Window::CursorEnterCallback(GLFWwindow* handle, int entered) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "MouseEnterEvent: " << (entered ? "TRUE" : "FALSE");

        window.selected = entered;
        window.onMouseEnter.publish(entered);
    }

    void Window::CharCallback(GLFWwindow* handle, unsigned int keycode) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "CharInputEvent: " << keycode;

        window.onCharInput.publish(keycode);
    }

    #if GLFW_VERSION_MINOR >= 1
    void Window::FileDropCallback(GLFWwindow* handle, int count, const char** paths) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "FileDropEvent: " << count;

        std::vector<std::filesystem::path> result;
        result.reserve(count);

        for (int i = 0; i < count; ++i) {
            result.emplace_back(paths[i]);
        }

        window.onFileDrop.publish(result);
    }
    #endif

    #if GLFW_VERSION_MINOR >= 3
    void Window::MaximizeCallback(GLFWwindow* handle, int maximized) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        LOG_VERBOSE << "MaximizeEvent: " << (maximized ? "TRUE" : "FALSE");

        window.onMaximize.publish(maximized);
    }

    void Window::ContentScaleCallback(GLFWwindow* handle, float scaleX, float scaleY) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        glm::vec2 scale {scaleX, scaleY};

        LOG_VERBOSE << "ContentScaleEvent: " << glm::to_string(scale);

        window.onContentScale.publish(scale);
    }
}
#endif