#include "glfw_window.h"

#include "fusion/bitmaps/bitmap.h"
#include "fusion/devices/device_manager.h"
#include "fusion/devices/cursor.h"

#include <GLFW/glfw3.h>

using namespace fe::glfw;

Window::Window(const VideoMode& videoMode, const WindowInfo& windowInfo) : fe::Window{}
    , size{windowInfo.size}
    , title{windowInfo.title}
    , borderless{windowInfo.isBorderless}
    , resizable{windowInfo.isResizable}
    , floating{windowInfo.isFloating}
    , fullscreen{windowInfo.isFullscreen}
    , vsync{windowInfo.isVSync} {

    FE_LOG_INFO("Creating window: '{}' [{} {}]", title, size.x, size.y);

    // Create a windowed mode window and its context.
    window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

    // Gets any window errors.
    if (!window)
        throw std::runtime_error("GLFW failed to create the window!");

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

void Window::onUpdate() {
    float dt = Time::DeltaTime().asSeconds();

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

void Window::setFullscreen(bool flag, const Monitor* monitor) {
    auto selected = monitor ? monitor : getCurrentMonitor();
    auto videoMode = selected->getVideoMode();

    fullscreen = flag;

    if (fullscreen) {
        FE_LOG_DEBUG("Window is going fullscreen");
        fullscreenSize = {videoMode.width, videoMode.height};
        glfwSetWindowMonitor(window, static_cast<GLFWmonitor*>(selected->getNativeMonitor()), 0, 0, fullscreenSize.x, fullscreenSize.y, GLFW_DONT_CARE);
    } else {
        FE_LOG_DEBUG("Window is going windowed");
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
    } else if (visible && !flag) {
        glfwHideWindow(window);
    }
    visible = flag;
    onShow.publish(flag);
}

bool Window::isClose() const {
    return glfwWindowShouldClose(window);
};

void Window::setClose(bool flag) {
    glfwSetWindowShouldClose(window, flag);
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

void Window::setTitle(std::string_view str) {
    title = str;
    glfwSetWindowTitle(window, title.c_str());
    onTitleChange.publish(title);
}

const char* Window::getClipboard() const {
    return glfwGetClipboardString(window);
}

void Window::setClipboard(std::string_view str) {
    glfwSetClipboardString(window, str.data());
}

void Window::setCursorHidden(bool hidden) {
    if (cursorHidden != hidden) {
        glfwSetInputMode(window, GLFW_CURSOR, hidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        if (!hidden && cursorHidden)
            setMousePosition(mousePosition);

        cursorHidden = hidden;
    }
}

void Window::setCursor(const Cursor* cursor) {
    glfwSetCursor(window, cursor ? static_cast<GLFWcursor*>(cursor->getNativeCursor()) : nullptr);
}

void Window::setIcons(gsl::span<const fs::path> filepaths) {
    std::vector<std::unique_ptr<Bitmap>> bitmaps;
    bitmaps.reserve(filepaths.size());

    std::vector<GLFWimage> icons;
    icons.reserve(filepaths.size());

    for (const auto& filepath : filepaths) {
        auto bitmap = std::make_unique<Bitmap>(filepath);
        if (!bitmap)
            continue;

        auto& icon = icons.emplace_back();
        icon.width = bitmap->getWidth();
        icon.height = bitmap->getHeight();
        icon.pixels = bitmap->getData<unsigned char>();
        bitmaps.push_back(std::move(bitmap));
    }

    glfwSetWindowIcon(window, static_cast<int>(icons.size()), icons.data());
}

fe::InputAction Window::getKey(Key key) const {
    return static_cast<InputAction>(glfwGetKey(window, static_cast<int>(key)));
}

fe::InputAction Window::getMouseButton(MouseButton button) const {
    return static_cast<InputAction>(glfwGetMouseButton(window, static_cast<int>(button)));
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

VkResult Window::createSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const {
    return glfwCreateWindowSurface(instance, window, allocator, surface);
}

int overlappingArea(const glm::ivec2& l1, const glm::ivec2& r1, const glm::ivec2& l2, const glm::ivec2& r2) {
    int area1 = glm::abs(l1.x - r1.x) * glm::abs(l1.y - r1.y);

    int area2 = glm::abs(l2.x - r2.x) * glm::abs(l2.y - r2.y);

    int areaI = (glm::min(r1.x, r2.x) - glm::max(l1.x, l2.x)) * (glm::min(r1.y, r2.y) - glm::max(l1.y, l2.y));

    return area1 + area2 - areaI;
}

const fe::Monitor* Window::getCurrentMonitor() const {
    if (fullscreen) {
        auto glfwMonitor = glfwGetWindowMonitor(window);
        for (const auto& monitor : DeviceManager::Get()->getMonitors()) {
            if (monitor->getNativeMonitor() == glfwMonitor)
                return monitor.get();
        }
        return nullptr;
    }

    std::multimap<int, const Monitor*> rankedMonitor;
    auto where = rankedMonitor.end();

    for (const auto& monitor : DeviceManager::Get()->getMonitors()) {
        where = rankedMonitor.insert(where, { overlappingArea(monitor->getWorkareaPosition(), monitor->getWorkareaPosition() + monitor->getWorkareaSize(), position, position + size), monitor.get() });
    }

    if (rankedMonitor.begin()->first >= 0)
        return rankedMonitor.begin()->second;

    return nullptr;
}

/* Events Callbacks */

namespace fe::glfw {

    void Window::PosCallback(GLFWwindow* handle, int x, int y) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::uvec2 pos {x, y};

        FE_LOG_VERBOSE("PositionChangeEvent: {}", pos);

        window.position = pos;
        window.onPositionChange.publish(pos);
    }

    void Window::SizeCallback(GLFWwindow* handle, int width, int height) {
        if (width <= 0 || height <= 0)
            return;

        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::uvec2 size {width, height};

        FE_LOG_VERBOSE("SizeChangeEvent: {}", size);

        if (window.fullscreen) {
            window.fullscreenSize = size;
        } else {
            window.size = size;
        }
        window.onResize.publish(size);
    }

    void Window::CloseCallback(GLFWwindow* handle) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("CloseEvent");

        window.onClose.publish();
    }

    void Window::RefreshCallback(GLFWwindow* handle) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("RefreshEvent");

        window.onRefresh.publish();
    }

    void Window::FocusCallback(GLFWwindow* handle, int focused) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("FocusEvent: {}", (focused ? "TRUE" : "FALSE"));

        window.focused = focused;
        window.onFocus.publish(focused);
    }

    void Window::IconifyCallback(GLFWwindow* handle, int iconified) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("IconifyEvent: {}", (iconified ? "TRUE" : "FALSE"));

        window.iconified = iconified;
        window.onIconify.publish(iconified);
    }

    void Window::FramebufferSizeCallback(GLFWwindow* handle, int width, int height) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::uvec2 size {width, height};

        FE_LOG_VERBOSE("FramebufferResize: {}{}", size);

        if (window.fullscreen) {
            window.fullscreenSize = size;
        } else {
            window.size = size;
        }
        window.onFramebufferResize.publish(size);
    }

    void Window::CursorPosCallback(GLFWwindow* handle, double posX, double posY) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::vec2 pos {posX, posY};

        FE_LOG_VERBOSE("MouseMotionEvent: {}", pos);

        window.mousePosition = pos;
        window.onMouseMotion.publish(pos);

        const auto& size = window.getSize();
        if (size.x <= 1 || size.y <= 1) {
            return;
        }

        glm::vec2 norm{ 2.0f * (pos / glm::vec2{size}) - 1.0f};

        FE_LOG_VERBOSE("MouseMotionNormEvent: {}", norm);

        window.mousePositionNorm = norm;
        window.onMouseMotionNorm.publish(norm);
    }

    void Window::ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::vec2 offset {offsetX, offsetY};

        FE_LOG_VERBOSE("MouseScrollEvent: {}", offset);

        window.mouseScroll = offset;
        window.onMouseScroll.publish(offset);
    }

    void Window::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("MouseButtonEvent: {}", glm::ivec3{button, action, mods});

        window.onMouseButton.publish(static_cast<MouseButton>(button), static_cast<InputAction>(action), bitmask::bitmask<InputMod>(static_cast<InputMod>(mods)));

        switch (action) {
            case GLFW_PRESS:
                window.onMousePress.publish(static_cast<MouseButton>(button));
                break;
            case GLFW_RELEASE:
                window.onMouseRelease.publish(static_cast<MouseButton>(button));
                break;
        }
    }

    void Window::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("KeyEvent: {}", glm::ivec4{key, scancode, action, mods});

        window.onKey.publish(static_cast<Key>(key), static_cast<InputAction>(action), static_cast<Key>(scancode), bitmask::bitmask<InputMod>(static_cast<InputMod>(mods)));

        switch (action) {
            case GLFW_PRESS:
                window.onKeyPress.publish(static_cast<Key>(key));
                break;
            case GLFW_RELEASE:
                window.onKeyRelease.publish(static_cast<Key>(key));
                break;
            case GLFW_REPEAT:
                window.onKeyHold.publish(static_cast<Key>(key));
                break;
        }
    }

    void Window::CursorEnterCallback(GLFWwindow* handle, int entered) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("MouseEnterEvent: {}", (entered ? "TRUE" : "FALSE"));

        window.selected = entered;
        window.onMouseEnter.publish(entered);
    }

    void Window::CharCallback(GLFWwindow* handle, unsigned int keycode) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("CharInputEvent: {}", keycode);

        window.onCharInput.publish(keycode);
    }

    #if GLFW_VERSION_MINOR >= 1
    void Window::FileDropCallback(GLFWwindow* handle, int count, const char** paths) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("FileDropEvent: {}", count);

        window.onFileDrop.publish({ paths, static_cast<size_t>(count) });
    }
    #endif

    #if GLFW_VERSION_MINOR >= 3
    void Window::MaximizeCallback(GLFWwindow* handle, int maximized) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        FE_LOG_VERBOSE("MaximizeEvent: {}", (maximized ? "TRUE" : "FALSE"));

        window.onMaximize.publish(maximized);
    }

    void Window::ContentScaleCallback(GLFWwindow* handle, float scaleX, float scaleY) {
        auto& window = *static_cast<Window*>(glfwGetWindowUserPointer(handle));

        glm::vec2 scale {scaleX, scaleY};

        FE_LOG_VERBOSE("ContentScaleEvent: {}", scale);

        window.onContentScale.publish(scale);
    }
}
#endif