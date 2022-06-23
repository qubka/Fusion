#include "cursor.hpp"

#include "fusion/renderer/bitmap.hpp"

using namespace glfw;

Cursor::Cursor(const std::filesystem::path& filename, fe::CursorHotspot hotspot) {
    fe::Bitmap bitmap{filename};
    if (!bitmap)
        return;

    GLFWimage image = {};
    image.width = bitmap.getWidth();
    image.height = bitmap.getHeight();
    image.pixels = bitmap.getData<uint8_t>();

    switch (hotspot) {
        case fe::CursorHotspot::UpperLeft:
            cursor = glfwCreateCursor(&image, 0, 0);
            break;
        case fe::CursorHotspot::UpperRight:
            cursor = glfwCreateCursor(&image, image.width - 1, 0);
            break;
        case fe::CursorHotspot::BottomLeft:
            cursor = glfwCreateCursor(&image, 0, image.height - 1);
            break;
        case fe::CursorHotspot::BottomRight:
            cursor = glfwCreateCursor(&image, image.width - 1, image.height - 1);
            break;
        case fe::CursorHotspot::Centered:
            cursor = glfwCreateCursor(&image, image.width / 2, image.height / 2);
            break;
    }
}

Cursor::Cursor(fe::CursorStandard standard) {
    cursor = glfwCreateStandardCursor(0x00036000 | static_cast<int>(standard));
}

Cursor::~Cursor() {
    glfwDestroyCursor(cursor);
}
