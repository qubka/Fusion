#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace fe {
    class Image2d;
    class Image2dArray;
    class ImageCube;
}

namespace ImGuiUtils {
    enum class Theme {
        Black,
        Dark,
        Dracula,
        Grey,
        Light,
        Blue,
        ClassicLight,
        ClassicDark,
        Classic,
        Cherry,
        Cinder
    };

    enum class PropertyFlag : uint8_t {
        None = 0,
        ColorProperty = 1,
        ReadOnly = 2,
        DragValue = 4,
        SliderValue = 8,
    };
    BITMASK_DEFINE_MAX_ELEMENT(PropertyFlag, SliderValue);

    using Flags = bitmask::bitmask<PropertyFlag>;

    bool Property(const std::string& name, std::string& value, const Flags& flags = PropertyFlag::ReadOnly);
    void PropertyConst(const std::string& name, const std::string& value);
    bool Property(const std::string& name, bool& value, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, int& value, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, uint32_t& value, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, double& value, double min = -1.0, double max = 1.0, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, int& value, int min = 0, int max = 100.0, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec2& value, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec3& value, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec4& value, bool exposeW, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, bool exposeW = false, const Flags& flags = PropertyFlag::None);
    bool Property(const std::string& name, glm::quat& value, const Flags& flags = PropertyFlag::None);

    bool PropertyControl(const std::string& name, glm::vec2& value, float min = 0.0f, float max = 0.0f, float reset = 0.0f, float speed = 0.1f);
    bool PropertyControl(const std::string& name, glm::vec3& value, float min = 0.0f, float max = 0.0f, float reset = 0.0f, float speed = 0.1f);

    template<typename E>
    bool PropertyDropdown(const std::string& name, E& value, const Flags& flags = PropertyFlag::None);
    bool PropertyDropdown(const std::string& name, std::string* options, int32_t optionCount, int32_t* selected, const Flags& flags = PropertyFlag::None);

    void Tooltip(const std::string& text);
    void Tooltip(const char* text);

    void Tooltip(fe::Image2d* texture, const glm::vec2& size);
    void Tooltip(fe::Image2d* texture, const glm::vec2& size, const std::string& text);
    void Tooltip(fe::Image2dArray* texture, uint32_t index, const glm::vec2& size);

    void Image(fe::Image2d* texture, const glm::vec2& size);
    void Image(fe::ImageCube* texture, const glm::vec2& size);
    void Image(fe::Image2dArray* texture, uint32_t index, const glm::vec2& size);

    void SetTheme(Theme theme);

    const glm::vec4& GetSelectedColor();
    const glm::vec4& GetIconColor();

    bool BufferingBar(const std::string& name, float value, const glm::vec2& size_arg, uint32_t bg_col, uint32_t fg_col);
    bool Spinner(const std::string& name, float radius, int thickness, uint32_t color);

    void DrawRowsBackground(int row_count, float line_height, float x1, float x2, float y_offset, uint32_t col_even, uint32_t col_odd);

    void DrawItemActivityOutline(float rounding = 0.0f, bool drawWhenInactive = false, const ImColor& colorWhenActive = ImColor{80, 80, 80});
    bool InputText(std::string& currentText);

    void AlternatingRowsBackground(float lineHeight = -1.0f);

    class ScopedStyle {
    public:
        template<typename T>
        ScopedStyle(ImGuiStyleVar styleVar, T value) { ImGui::PushStyleVar(styleVar, value); }
        ~ScopedStyle() { ImGui::PopStyleVar(); }
        NONCOPYABLE(ScopedStyle);
    };

    class ScopedColor {
    public:
        template<typename T>
        ScopedColor(ImGuiCol colorId, T color) { ImGui::PushStyleColor(colorId, color); }
        ~ScopedColor() { ImGui::PopStyleColor(); }
        NONCOPYABLE(ScopedColor);
    };

    class ScopedFont {
    public:
        ScopedFont(ImFont* font) { ImGui::PushFont(font); }
        ~ScopedFont() { ImGui::PopFont(); }
        NONCOPYABLE(ScopedFont);
    };

    class ScopedID {
    public:
        template<typename T>
        ScopedID(T id) { ImGui::PushID(id); }
        ~ScopedID() { ImGui::PopID(); }
        NONCOPYABLE(ScopedID);
    };
};

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}

static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) {
    return {lhs.x / rhs, lhs.y / rhs};
}

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}

static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
}

static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}

static inline ImVec4 operator*(const ImVec4& lhs, const float rhs) {
    return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs};
}

static inline ImVec4 operator/(const ImVec4& lhs, const float rhs) {
    return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs};
}

static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w};
}

static inline ImVec4 operator/(const ImVec4& lhs, const ImVec4& rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
}

static inline ImVec4& operator+=(ImVec4& lhs, const ImVec4& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}

static inline ImVec4& operator-=(ImVec4& lhs, const ImVec4& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}

static inline ImVec4& operator*=(ImVec4& lhs, const float rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}

static inline ImVec4& operator/=(ImVec4& lhs, const float rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}

static inline std::ostream& operator<<(std::ostream& ostream, const ImVec2 a) {
    ostream << "{ " << a.x << ", " << a.y << " }";
    return ostream;
}

static inline std::ostream& operator<<(std::ostream& ostream, const ImVec4 a) {
    ostream << "{ " << a.x << ", " << a.y << ", " << a.z << ", " << a.w << " }";
    return ostream;
}