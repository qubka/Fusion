#pragma once

#include "imgui_color_scheme.hpp"
#include "fusion/utils/string.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace fe {
    class Texture2d;
    class Texture2dArray;
    class TextureCube;
}

using namespace fe;

namespace ImGuiUtils {
    enum class Theme : uint8_t {
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
        Cinder,
        Steam
    };

    enum class PropertyFlag : uint8_t {
        None = 0,
        ReadOnly = 1,
        ColorValue = 2,
        DragValue = 4, //! default in edit mode
        SliderValue = 8,
    };
    BITMASK_DEFINE_MAX_ELEMENT(PropertyFlag, SliderValue);

    void TextCentered(const std::string& text, std::optional<float> offsetY = std::nullopt);

    void Tooltip(const std::string& text);
    void Tooltip(Texture2d* texture, const ImVec2& size, bool flipImage = false);
    void Tooltip(Texture2d* texture, const ImVec2& size, const std::string& text, bool flipImage = false);

    void Tooltip(Texture2dArray* texture, uint32_t index, const ImVec2& size, bool flipImage = false);
    void Image(Texture2d* texture, const ImVec2& size, bool flipImage = false);
    void Image(TextureCube* texture, const ImVec2& size, bool flipImage = false);
    void Image(Texture2dArray* texture, uint32_t index, const ImVec2& size, bool flipImage = false);

    void Image(uint32_t* texture_id, const ImVec2& size, bool flipImage = false);

    void SetTheme(Theme theme);
    bool BufferingBar(const std::string& name, float value, ImVec2 size, ImU32 bgColor, ImU32 fgColor);

    bool Spinner(const std::string& name, float radius, int thickness, ImU32 color);
    bool ToggleRoundButton(const std::string& name, bool& value);

    bool ToggleButton(const std::string& name, bool& value, bool text_style = false); // if text true, select text on active

    void DrawRowsBackground(int rowCount, float lineHeight, float x1, float x2, float yOffset, ImU32 colEven, ImU32 colOdd);
    void DrawItemActivityOutline(float rounding = 0.0f, bool drawWhenInactive = false, ImU32 colorWhenActive = IM_COL32(80, 80, 80, 255));

    bool InputText(const std::string& name, std::string& currentText);

    void AlternatingRowsBackground(float lineHeight = -1.0f);

    template<typename T>
    constexpr ImGuiDataType_ GetDataType() {
        if constexpr (std::is_same_v<T, int8_t>) {
            return ImGuiDataType_S8;
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            return ImGuiDataType_U8;
        } else if constexpr (std::is_same_v<T, int16_t>) {
            return ImGuiDataType_S16;
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            return ImGuiDataType_U16;
        } else if constexpr (std::is_same_v<T, int32_t>) {
            return ImGuiDataType_S32;
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            return ImGuiDataType_U32;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return ImGuiDataType_S64;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return ImGuiDataType_U64;
        } else if constexpr (std::is_same_v<T, float>) {
            return ImGuiDataType_Float;
        } else if constexpr (std::is_same_v<T, double>) {
            return ImGuiDataType_Double;
        } else {
            static_assert("Invalid type");
        }
        return ImGuiDataType_COUNT;
    }

    using Flags = bitmask::bitmask<PropertyFlag>;

    void PropertyText(const std::string& name, const std::string& value);
    bool PropertyText(const std::string& name, std::string& value, const Flags& flags = PropertyFlag::None);

    template<typename T, typename = std::enable_if_t<std::is_same_v<T, bool>>>
    bool Property(const std::string& name, T& value, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        //ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        if (flags & PropertyFlag::ReadOnly) {
            ImGui::TextUnformatted(value ? glm::detail::LabelTrue : glm::detail::LabelFalse);
        } else {
            std::string id{ "##" + name };
            if (ImGui::Checkbox(id.c_str(), &value))
                updated = true;
        }

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return updated;
    }

    template<typename T, typename = std::enable_if_t<!std::is_same_v<T, bool>>>
    bool Property(const std::string& name, T& value, const T min = 0, const T max = 0, float speed = 1.0f, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        if (flags & PropertyFlag::ReadOnly) {
            ImGui::Text(ImGui::DataTypeGetInfo(GetDataType<T>())->PrintFmt, value);
        } else if (flags & PropertyFlag::SliderValue) {
            std::string id{ "##" + name };
            if (ImGui::SliderScalar(id.c_str(), GetDataType<T>(), &value, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp))
                updated = true;
        } else {
            std::string id{ "##" + name };
            if (ImGui::DragScalar(id.c_str(), GetDataType<T>(), &value, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp))
                updated = true;
        }

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return updated;
    }

    template<glm::length_t L, typename T, glm::qualifier Q, typename = std::enable_if_t<std::is_same_v<T, bool>>>
    bool Property(const std::string& name, glm::vec<L, T, Q>& value, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        //ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        if (flags & PropertyFlag::ReadOnly) {
            ImGui::TextUnformatted(String::Extract(glm::to_string(value), "(", ")").c_str());
        } else {
            for (int i = 0; i < L; i++) {
                std::string id{ "##" + name + std::to_string(i) };
                if (ImGui::Checkbox(id.c_str(), &value[i]))
                    updated = true;
            }
        }

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return updated;
    }

    template<glm::length_t L, typename T, glm::qualifier Q, typename = std::enable_if_t<!std::is_same_v<T, bool>>>
    bool Property(const std::string& name, glm::vec<L, T, Q>& value, const T min = 0, const T max = 0, float speed = 1.0f, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        if (flags & PropertyFlag::ReadOnly) {
            if (flags & PropertyFlag::ColorValue) {
                std::string id{ "##" + name };
                glm::vec<L, T, Q> copy{ value };
                if constexpr (L == 3) {
                    ImGui::ColorEdit3(id.c_str(), glm::value_ptr(copy), ImGuiColorEditFlags_NoInputs);
                } else if constexpr (L == 4) {
                    ImGui::ColorEdit4(id.c_str(), glm::value_ptr(copy), ImGuiColorEditFlags_NoInputs);
                }
            } else {
                ImGui::TextUnformatted(String::Extract(glm::to_string(value), "(", ")").c_str());
            }
        } else if (flags & PropertyFlag::ColorValue) {
            std::string id{ "##" + name };
            if constexpr (L == 3) {
                if (ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
                    updated = true;
            } else if constexpr (L == 4) {
                if (ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
                    updated = true;
            }
        } else if (flags & PropertyFlag::SliderValue) {
            std::string id{ "##" + name };
            if (ImGui::SliderScalarN(id.c_str(), GetDataType<T>(), glm::value_ptr(value), L, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp))
                updated = true;
        } else {
            std::string id{ "##" + name };
            if (ImGui::DragScalarN(id.c_str(), GetDataType<T>(), glm::value_ptr(value), L, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp))
                updated = true;
        }

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return updated;
    }

    template<glm::length_t L, typename T, glm::qualifier Q, typename = std::enable_if_t<!std::is_same_v<T, bool>>>
    bool PropertyControl(const std::string& name, glm::vec<L, T, Q>& value, const T min = 0, const T max = 0, T reset = 0, float speed = 1.0f, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushID(name.c_str());

        //ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();

        ImGuiDataType_ dataType = GetDataType<T>();
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };

        ImGui::PushMultiItemsWidths(L, ImGui::GetColumnWidth() - L * lineHeight); // ImGui::CalcItemWidth()
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

       if constexpr(L > 0) {
            ImVec4 color{ 0.8f, 0.1f, 0.15f, 1.0f };
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            if (ImGui::Button("X", buttonSize)) {
                value.x = reset;
                updated |= true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            updated |= ImGui::DragScalar("##X", dataType, &value.x, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();
        }

        if constexpr(L > 1) {
            ImGui::SameLine();

            ImVec4 color{ 0.2f, 0.7f, 0.2f, 1.0f };
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            if (ImGui::Button("Y", buttonSize)) {
                value.y = reset;
                updated |= true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            updated |= ImGui::DragScalar("##Y", dataType, &value.y, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();
        }

        if constexpr(L > 2) {
            ImGui::SameLine();

            ImVec4 color{ 0.1f, 0.25f, 0.8f, 1.0f };
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            if (ImGui::Button("Z", buttonSize)) {
                value.z = reset;
                updated |= true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            updated |= ImGui::DragScalar("##Z", dataType, &value.z, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();
        }

        if constexpr(L > 3) {
            ImGui::SameLine();

            ImVec4 color{ 0.8f, 0.2f, 0.8f, 1.0f };
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            if (ImGui::Button("W", buttonSize)) {
                value.w = reset;
                updated |= true;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            updated |= ImGui::DragScalar("##W", dataType, &value.w, speed, &min, &max, nullptr, ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();
        }

        ImGui::PopStyleVar();

        ImGui::NextColumn();

        ImGui::PopID();

        return updated;
    }

    bool PropertyDropdown(const std::string& name, std::string* options, int32_t optionCount, int32_t* selected);
    template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
    bool PropertyDropdown(const std::string& name, E& value, const Flags& flags = PropertyFlag::None) {
        bool updated = false;

        //ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        constexpr auto entries = me::enum_entries<E>();
        const char* current = entries[me::enum_index(value).value_or(0)].second.data();

        if (flags & PropertyFlag::ReadOnly) {
            ImGui::TextUnformatted(current);
        } else {
            std::string id{ "##" + name };
            if (ImGui::BeginCombo(id.c_str(), current)) {
                for (const auto& [type, title] : entries) {
                    const bool is_selected = (value == type);
                    if (ImGui::Selectable(title.data(), is_selected)) {
                        value = type;
                        updated = true;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        DrawItemActivityOutline(2.5f);

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return updated;
    }

    bool PropertyFile(const std::string& name, const fs::path& path, fs::path& value, std::vector<fs::path>& files, fs::path& selected, ImGuiTextFilter& filter);
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

static inline std::ostream& operator<<(std::ostream& ostream, const ImVec2& v) {
    ostream << "{ " << v.x << ", " << v.y << " }";
    return ostream;
}

static inline std::ostream& operator<<(std::ostream& ostream, const ImVec4& v) {
    ostream << "{ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " }";
    return ostream;
}