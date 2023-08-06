#include "glfw_cursor.h"

#include "fusion/bitmaps/bitmap.h"

#include <GLFW/glfw3.h>

using namespace fe::glfw;

Cursor::Cursor(fs::path filepath, CursorHotspot hotspot) : path{std::move(filepath)}, hotspot{hotspot} {
    Bitmap bitmap{path};
    if (!bitmap)
        return;

    GLFWimage image = {};
    image.width = bitmap.getWidth();
    image.height = bitmap.getHeight();
    image.pixels = bitmap.getData<unsigned char>();

    switch (hotspot) {
        case CursorHotspot::UpperLeft:
            cursor = glfwCreateCursor(&image, 0, 0);
            break;
        case CursorHotspot::UpperRight:
            cursor = glfwCreateCursor(&image, image.width - 1, 0);
            break;
        case CursorHotspot::BottomLeft:
            cursor = glfwCreateCursor(&image, 0, image.height - 1);
            break;
        case CursorHotspot::BottomRight:
            cursor = glfwCreateCursor(&image, image.width - 1, image.height - 1);
            break;
        case CursorHotspot::Centered:
            cursor = glfwCreateCursor(&image, image.width / 2, image.height / 2);
            break;
    }
}

Cursor::Cursor(CursorStandard standard) : standard{standard} {
    if (standard == CursorStandard::None) standard = CursorStandard::Arrow;
    cursor = glfwCreateStandardCursor(0x00036000 | (static_cast<int>(standard)));
}

Cursor::~Cursor() {
    glfwDestroyCursor(cursor);
}
