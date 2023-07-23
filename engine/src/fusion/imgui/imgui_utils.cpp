#include "imgui_utils.h"

#include "fusion/graphics/textures/texture2d.h"
#include "fusion/graphics/textures/texture2d_array.h"
#include "fusion/graphics/textures/texture_cube.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace ImGuiUtils {
using namespace fe;
using namespace std::string_literals;

void TextCentered(const char* text, std::optional<float> offsetY) {
    ImVec2 windowSize{ ImGui::GetWindowSize() };
    ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize(text).x) * 0.5f);
    if (offsetY) {
        ImGui::SetCursorPosY(*offsetY + windowSize.y * 0.5f);
    }
    ImGui::TextUnformatted(text);
}

bool PropertyText(const char* name, std::string& value) {
    bool updated = false;

    //ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    ImGui::PushID(name);

    static char buffer[256]{};
    std::strncpy(buffer, value.c_str(), sizeof(buffer));
    if (ImGui::InputText("", buffer, sizeof(buffer), ImGuiInputTextFlags_AutoSelectAll)) {
        value = std::string{buffer};
        updated = true;
    }

    ImGui::PopID();

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

void PropertyText(const char* name, const char* value) {
    //ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    ImGui::TextUnformatted(value);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

bool PropertyDropdown(const char* name, std::span<const char*> options, int32_t& selected) {
    bool updated = false;

    //ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name);
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    ImGui::PushID(name);

    const char* current = options[selected];

    if (ImGui::BeginCombo("", current)) {
        for (const auto& [i, option] : enumerate(options)) {
            const bool is_selected = (current == option);
            if (ImGui::Selectable(option, is_selected)) {
                current = option;
                selected = i;
                updated = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::PopID();

    DrawItemActivityOutline(2.5f);

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

/*bool PropertyFile(const char* name, const fs::path& path, fs::path& value, std::vector<fs::path>& files, fs::path& selected, ImGuiTextFilter& filter) {
    bool updated = false;

    if (files.empty()) {
        //files = FileSystem::GetFilesInPath("", true);
        //LOG_WARNING << "Folder seems to be empty!";
    }

    ImGui::TextUnformatted(name);
    ImGui::NextColumn();

    float yPadding = ImGui::GetStyle().FramePadding.y;
    float lineHeight = ImGui::GetFontSize() + yPadding * 2.0f;

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{4, 4});

    if (ImGui::BeginPopup("FileExplorer", ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Dummy(ImVec2{200.0f, 0.0f});  // fix resize
        ImGui::TextUnformatted(ICON_MDI_FILE_SEARCH);
        ImGui::SameLine();

        ImGuiIO& io = ImGui::GetIO();

        {
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
            filter.Draw("###FileFilter");
            DrawItemActivityOutline(2.0f, false);
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }

        if (!filter.IsActive()) {
            ImGui::SameLine();
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::SetCursorPosX(ImGui::GetFontSize() * 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0.0f, yPadding});
            ImGui::TextUnformatted("Search...");
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }

        ImGui::Separator();

        ImGui::BeginChild("FileBrowser", ImVec2{300.0f, 500.0f});

        for (const auto& file : files) {
            if (filter.IsActive() && !filter.PassFilter(file.c_str()))
                continue;

            std::string title{ FileFormat::GetIcon(file) + " "s + file.filename().string() };
            if (ImGui::Selectable(title.c_str(), selected == file, ImGuiSelectableFlags_AllowDoubleClick)) {
                selected = file;
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    value = file;
                    updated = true;
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::EndChild();
        ImGui::Separator();

        if (!selected.empty()) {
            std::string title{ FileFormat::GetIcon(selected) + " "s + selected.string() };
            ImGui::TextUnformatted(title.c_str());
        }

        ImGui::EndPopup();
    }

    ImVec2 buttonSize{ ImGui::GetContentRegionAvail().x - lineHeight, lineHeight };

    if (value.empty()) {
        if (ImGui::Button("...", buttonSize)) {
            filter.Clear();
            selected = "";
            //files = FileSystem::GetFilesInPath("", true);
            ImGui::OpenPopup("FileExplorer");
        }
    } else {
        std::string title{ FileFormat::GetIcon(value) + " "s + value.filename().string() };
        if (ImGui::Button(title.c_str(), buttonSize)) {
            //contentBrowserPanel.selectFile(value);
            // TODO: Finish
        }
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        value = "";
        updated = true;
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            value = static_cast<const char*>(payload->Data);
            updated = true;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    if (ImGui::Button(ICON_MDI_FILE_SEARCH)) {
        filter.Clear();
        selected = "";
        //files = FileSystem::GetFilesInPath("", true);
        ImGui::OpenPopup("FileExplorer");
    }

    ImGui::PopStyleVar();

    return updated;
}*/

void Tooltip(const char* text) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(text);
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Texture2d* texture, const ImVec2& size, bool flipImage) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Texture2d* texture, const ImVec2& size, const char* text, bool flipImage) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();

        ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
        ImGui::TextUnformatted(text);
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Texture2dArray* texture, uint32_t index, const ImVec2& size, bool flipImage) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();

        ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Image(Texture2d* texture, const ImVec2& size, bool flipImage) {
    ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
}

void Image(TextureCube* texture, const ImVec2& size, bool flipImage) {
    ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
}

void Image(Texture2dArray* texture, uint32_t index, const ImVec2& size, bool flipImage) {
    ImGui::Image(texture, size, ImVec2{0.0f, flipImage ? 1.0f : 0.0f}, ImVec2{1.0f, flipImage ? 0.0f : 1.0f});
}

bool BufferingBar(const char* name, float value, ImVec2 size, ImU32 bgColor, ImU32 fgColor) {
    ImGuiContext* g = ImGui::GetCurrentContext();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImGuiStyle& style = ImGui::GetStyle();
    const ImGuiID id = ImGui::GetID(name);

    ImVec2 pos{ ImGui::GetCursorPos() };
    size.x -= style.FramePadding.x * 2;

    const ImRect bb{pos, pos + size};
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    const float circleStart = size.x * 0.7f;
    const float circleEnd = size.x;
    const float circleWidth = circleEnd - circleStart;

    drawList->AddRectFilled(bb.Min, ImVec2{pos.x + circleStart, bb.Max.y}, bgColor);
    drawList->AddRectFilled(bb.Min, ImVec2{pos.x + circleStart * value, bb.Max.y}, fgColor);

    const float t = static_cast<float>(g->Time);
    const float r = size.y * 0.5f;
    const float speed = 1.5f;

    const float a = speed * 0.f;
    const float b = speed * 0.333f;
    const float c = speed * 0.666f;

    const float o1 = (circleWidth + r) * (t + a - speed * glm::round((t + a) / speed)) / speed;
    const float o2 = (circleWidth + r) * (t + b - speed * glm::round((t + b) / speed)) / speed;
    const float o3 = (circleWidth + r) * (t + c - speed * glm::round((t + c) / speed)) / speed;

    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o1, bb.Min.y + r}, r, bgColor);
    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o2, bb.Min.y + r}, r, bgColor);
    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o3, bb.Min.y + r}, r, bgColor);

    return true;
}

bool Spinner(const char* name, float radius, int thickness, ImU32 color) {
    ImGuiContext* g = ImGui::GetCurrentContext();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImGuiStyle& style = g->Style;
    const ImGuiID id = ImGui::GetID(name);

    ImVec2 pos{ ImGui::GetCursorPos() };
    ImVec2 size{ radius * 2, (radius + style.FramePadding.y) * 2 };

    const ImRect bb{pos, ImVec2{pos.x + size.x, pos.y + size.y}};
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    drawList->PathClear();

    const float numSegments = 30.0f;
    float start = abs(glm::sin(static_cast<float>(g->Time) * 1.8f) * numSegments - 5);

    const float min = glm::pi<float>() * 2.0f * (start / numSegments);
    const float max = glm::pi<float>() * 2.0f * (numSegments - 3.0f) / numSegments;

    const ImVec2 centre{pos.x + radius, pos.y + radius + style.FramePadding.y};

    for (int i = 0; i < static_cast<int>(numSegments); i++) {
        const float a = min + (static_cast<float>(i) / numSegments) * (max - min);
        drawList->PathLineTo(ImVec2{centre.x + glm::cos(a + static_cast<float>(g->Time) * 8) * radius,
                                    centre.y + glm::sin(a + static_cast<float>(g->Time) * 8) * radius});
    }

    drawList->PathStroke(color, false, static_cast<float>(thickness));

    return true;
}

// https://gist.github.com/moebiussurfing/c1110be8bb3f6776311512b63523a0a3
bool ToggleRoundButton(const char* name, bool& value) {
    bool updated = false;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos{ ImGui::GetCursorScreenPos() };

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(name, ImVec2{width, height});
    if (ImGui::IsItemClicked()) {
        value = !value;
        updated = true;
    }

    float t = value ? 1.0f : 0.0f;

    ImGuiContext* g = ImGui::GetCurrentContext();
    float ANIM_SPEED = 0.08f;
    if (g->LastActiveId == g->CurrentWindow->GetID(name)) {
        float t_anim = ImSaturate(g->LastActiveIdTimer / ANIM_SPEED);
        t = value ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col = ImGui::IsItemHovered() ?
        ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg), ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered), t))
    :
        ImGui::GetColorU32(ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_Button), ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered), t));

    drawList->AddRectFilled(pos, ImVec2{pos.x + width, pos.y + height}, col, height * 0.5f);
    drawList->AddCircleFilled(ImVec2{pos.x + radius + t * (width - radius * 2.0f), pos.y + radius}, radius - 1.5f, IM_COL32(255, 255, 255, 255));

    return updated;
}

bool ToggleButton(const char* name, bool& value, bool textStyle) {
    bool updated = false;
    if (value) {
        if (textStyle) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
        } else {
            ImVec4 color{ ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) };
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
        }
        if (ImGui::Button(name)) {
            value = !value;
            updated = true;
        }
        ImGui::PopStyleColor(textStyle ? 1 : 3);
    } else {
        if (ImGui::Button(name)) {
            value = true;
            updated = true;
        }
    }
    return updated;
}

void DrawRowsBackground(int rowCount, float lineHeight, float x1, float x2, float yOffset, ImU32 colEven, ImU32 colOdd) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float y0 = ImGui::GetCursorScreenPos().y + glm::round(yOffset);

    ImGuiListClipper clipper;
    clipper.Begin(rowCount, lineHeight);
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            ImU32 col = (i & 1) ? colOdd : colEven;
            if ((col & IM_COL32_A_MASK) == 0)
                continue;
            float y1 = y0 + (lineHeight * static_cast<float>(i));
            float y2 = y1 + lineHeight;
            drawList->AddRectFilled(ImVec2{x1, y1}, ImVec2{x2, y2}, col);
        }
    }
}

void DrawItemActivityOutline(float rounding, bool drawWhenInactive, ImU32 colorWhenActive) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImRect rect{ ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
    rect.Min.x -= 1.0f;
    rect.Min.y -= 1.0f;
    rect.Max.x += 1.0f;
    rect.Max.y += 1.0f;

    if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max, IM_COL32(60, 60, 60, 255), rounding, 0, 1.5f);
    }
    if (ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max, colorWhenActive, rounding, 0, 1.0f);
    } else if (!ImGui::IsItemHovered() && drawWhenInactive) {
        drawList->AddRect(rect.Min, rect.Max, IM_COL32(50, 50, 50, 255), rounding, 0, 1.0f);
    }
}

bool InputText(const char* name, std::string& currentText) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});

    static char buffer[256]{};
    std::strncpy(buffer, currentText.c_str(), sizeof(buffer));

    bool updated = ImGui::InputText(name, buffer, 256);

    DrawItemActivityOutline(2.0f, false);

    if (updated)
        currentText = std::string{buffer};

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    return updated;
}

// from https://github.com/ocornut/imgui/issues/2668
void AlternatingRowsBackground(float lineHeight) {
    const ImU32 color = ImGui::GetColorU32(ImGuiCol_TableRowBgAlt);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGuiStyle& style = ImGui::GetStyle();

    if (lineHeight < 0) {
        lineHeight = ImGui::GetTextLineHeight();
    }

    lineHeight += style.ItemSpacing.y;

    float xScroll = ImGui::GetScrollX();
    float yScroll = ImGui::GetScrollY();
    float scrolledOutLines = glm::floor(yScroll / lineHeight);
    yScroll -= lineHeight * scrolledOutLines;

    ImVec2 clipRectMin{ImGui::GetWindowPos()};
    ImVec2 clipRectMax{clipRectMin.x + ImGui::GetWindowWidth(), clipRectMin.y + ImGui::GetWindowHeight()};

    if (ImGui::GetScrollMaxX() > 0) {
        clipRectMax.y -= style.ScrollbarSize;
    }

    drawList->PushClipRect(clipRectMin, clipRectMax);

    const float yMin = clipRectMin.y - yScroll + ImGui::GetCursorPosY();
    const float yMax = clipRectMax.y - yScroll + lineHeight;
    const float xMin = clipRectMin.x + xScroll + ImGui::GetWindowContentRegionMin().x;
    const float xMax = clipRectMin.x + xScroll + ImGui::GetWindowContentRegionMax().x;

    bool odd = (static_cast<int>(scrolledOutLines) % 2) == 0;
    for (float y = yMin; y < yMax; y += lineHeight, odd = !odd) {
        if (odd) {
            drawList->AddRectFilled(ImVec2{xMin, y - style.ItemSpacing.y}, ImVec2{xMax, y + lineHeight}, color);
        }
    }

    drawList->PopClipRect();
}

void SetTheme(Theme theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    if (theme == Theme::Black)
    {
        ImGui::StyleColorsDark();
        colors[ImGuiCol_Text] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f };
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.10f, 0.10f, 0.10f, 1.00f };
        colors[ImGuiCol_ChildBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_PopupBg] = ImVec4{ 0.19f, 0.19f, 0.19f, 0.92f };
        colors[ImGuiCol_Border] = ImVec4{ 0.19f, 0.19f, 0.19f, 0.29f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.24f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.05f, 0.05f, 0.05f, 0.54f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.19f, 0.19f, 0.54f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.20f, 0.22f, 0.23f, 1.00f };
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.06f, 0.06f, 0.06f, 1.00f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.14f, 0.14f, 0.14f, 1.00f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.05f, 0.05f, 0.05f, 0.54f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.34f, 0.34f, 0.34f, 0.54f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.40f, 0.40f, 0.40f, 0.54f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.56f, 0.56f, 0.56f, 0.54f };
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.33f, 0.67f, 0.86f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.34f, 0.34f, 0.34f, 0.54f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.56f, 0.56f, 0.56f, 0.54f };
        colors[ImGuiCol_Button] = ImVec4{ 0.05f, 0.05f, 0.05f, 0.54f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.19f, 0.19f, 0.54f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.20f, 0.22f, 0.23f, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.52f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.36f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.20f, 0.22f, 0.23f, 0.33f };
        colors[ImGuiCol_Separator] = ImVec4{ 0.28f, 0.28f, 0.28f, 0.29f };
        colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.44f, 0.44f, 0.44f, 0.29f };
        colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.40f, 0.44f, 0.47f, 1.00f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.28f, 0.28f, 0.28f, 0.29f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.44f, 0.44f, 0.44f, 0.29f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.40f, 0.44f, 0.47f, 1.00f };
        colors[ImGuiCol_Tab] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.52f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.14f, 0.14f, 0.14f, 1.00f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.20f, 0.20f, 0.20f, 0.36f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.52f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.14f, 0.14f, 0.14f, 1.00f };
        colors[ImGuiCol_DockingPreview] = ImVec4{ 0.33f, 0.67f, 0.86f, 1.00f };
        colors[ImGuiCol_DockingEmptyBg] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotLines] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_TableHeaderBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.52f };
        colors[ImGuiCol_TableBorderStrong] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.52f };
        colors[ImGuiCol_TableBorderLight] = ImVec4{ 0.28f, 0.28f, 0.28f, 0.29f };
        colors[ImGuiCol_TableRowBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_TableRowBgAlt] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.06f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.20f, 0.22f, 0.23f, 1.00f };
        colors[ImGuiCol_DragDropTarget] = ImVec4{ 0.33f, 0.67f, 0.86f, 1.00f };
        colors[ImGuiCol_NavHighlight] = ImVec4{ 1.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.00f, 0.00f, 0.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.35f };
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.35f };
    }
    else if (theme == Theme::Dark)
    {
        ImGui::StyleColorsDark();
        ImVec4 Titlebar{ 40.0f / 255.0f, 42.0f / 255.0f, 54.0f / 255.0f, 1.0f };
        ImVec4 TabActive{ 52.0f / 255.0f, 54.0f / 255.0f, 64.0f / 255.0f, 1.0f };
        ImVec4 TabUnactive{ 35.0f / 255.0f, 43.0f / 255.0f, 59.0f / 255.0f, 1.0f };

        colors[ImGuiCol_Text] = ImVec4{ 200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };

        colors[ImGuiCol_WindowBg] = TabActive;
        colors[ImGuiCol_ChildBg] = TabActive;

        colors[ImGuiCol_PopupBg] = ImVec4{ 42.0f / 255.0f, 38.0f / 255.0f, 47.0f / 255.0f, 1.00f };
        colors[ImGuiCol_Border] = ImVec4{ 0.08f, 0.10f, 0.12f, 1.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 65.0f / 255.0f, 79.0f / 255.0f, 92.0f / 255.0f, 1.00f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.12f, 0.20f, 0.28f, 1.00f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.09f, 0.12f, 0.14f, 1.00f };

        colors[ImGuiCol_TitleBg] = Titlebar;
        colors[ImGuiCol_TitleBgActive] = Titlebar;
        colors[ImGuiCol_TitleBgCollapsed] = Titlebar;
        colors[ImGuiCol_MenuBarBg] = Titlebar;

        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.02f, 0.02f, 0.02f, 0.39f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.6f, 0.6f, 0.6f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.7f, 0.7f, 0.7f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.8f, 0.8f, 0.8f, 1.00f };

        colors[ImGuiCol_CheckMark] = ImVec4{ 155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 185.0f / 255.0f, 160.0f / 255.0f, 237.0f / 255.0f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f } + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f } + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };

        colors[ImGuiCol_Separator] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f };
        colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.10f, 0.40f, 0.75f, 0.78f };
        colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.10f, 0.40f, 0.75f, 1.00f };

        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.25f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.67f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.95f };

        colors[ImGuiCol_PlotLines] = ImVec4{ 0.61f, 0.61f, 0.61f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.35f };
        colors[ImGuiCol_DragDropTarget] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_NavHighlight] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.20f };
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.35f };

        colors[ImGuiCol_Header] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_HeaderHovered] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_HeaderActive] = TabActive + ImVec4{ 0.05f, 0.05f, 0.05f, 0.1f };

#ifdef IMGUI_HAS_DOCK

        colors[ImGuiCol_Tab] = TabUnactive;
        colors[ImGuiCol_TabHovered] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_TabActive] = TabActive;
        colors[ImGuiCol_TabUnfocused] = TabUnactive;
        colors[ImGuiCol_TabUnfocusedActive] = TabActive;
        colors[ImGuiCol_DockingEmptyBg] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };
        colors[ImGuiCol_DockingPreview] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };

#endif
    }
    else if (theme == Theme::Grey)
    {
        ImGui::StyleColorsDark();
        colors[ImGuiCol_Text] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.40f, 0.40f, 0.40f, 1.00f };

        colors[ImGuiCol_ChildBg] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.00f };
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.00f };
        colors[ImGuiCol_PopupBg] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.00f };
        colors[ImGuiCol_Border] = ImVec4{ 0.12f, 0.12f, 0.12f, 0.71f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.06f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.42f, 0.42f, 0.42f, 0.54f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.42f, 0.42f, 0.42f, 0.40f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.56f, 0.56f, 0.56f, 0.67f };
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.19f, 0.19f, 0.19f, 1.00f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.22f, 0.22f, 0.22f, 1.00f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.17f, 0.17f, 0.17f, 0.90f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.335f, 0.335f, 0.335f, 1.000f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.24f, 0.24f, 0.24f, 0.53f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.41f, 0.41f, 0.41f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.52f, 0.52f, 0.52f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.76f, 0.76f, 0.76f, 1.00f };
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.65f, 0.65f, 0.65f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.52f, 0.52f, 0.52f, 1.00f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.64f, 0.64f, 0.64f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.54f, 0.54f, 0.54f, 0.35f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.52f, 0.52f, 0.52f, 0.59f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.76f, 0.76f, 0.76f, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.00f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.47f, 0.47f, 0.47f, 1.00f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.76f, 0.76f, 0.76f, 0.77f };
        colors[ImGuiCol_Separator] = ImVec4{ 0.000f, 0.000f, 0.000f, 0.137f };
        colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.700f, 0.671f, 0.600f, 0.290f };
        colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.702f, 0.671f, 0.600f, 0.674f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.25f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.67f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.95f };
        colors[ImGuiCol_PlotLines] = ImVec4{ 0.61f, 0.61f, 0.61f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.73f, 0.73f, 0.73f, 0.35f };
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.35f };
        colors[ImGuiCol_DragDropTarget] = ImVec4{ 1.00f, 1.00f, 0.00f, 0.90f };
        colors[ImGuiCol_NavHighlight] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.20f };

#ifdef IMGUI_HAS_DOCK
        colors[ImGuiCol_DockingEmptyBg] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.00f };
        colors[ImGuiCol_Tab] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.00f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.40f, 0.40f, 0.40f, 1.00f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.00f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };
        colors[ImGuiCol_DockingPreview] = ImVec4{ 0.85f, 0.85f, 0.85f, 0.28f };
#endif
    }
    else if (theme == Theme::Light)
    {
        ImGui::StyleColorsLight();
        colors[ImGuiCol_Text] = ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.60f, 0.60f, 0.60f, 1.00f };

        colors[ImGuiCol_WindowBg] = ImVec4{ 0.94f, 0.94f, 0.94f, 0.94f };
        colors[ImGuiCol_PopupBg] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.94f };
        colors[ImGuiCol_Border] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.39f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.10f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.94f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.40f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.67f };
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.96f, 0.96f, 0.96f, 1.00f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.51f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.82f, 0.82f, 0.82f, 1.00f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.86f, 0.86f, 0.86f, 1.00f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.98f, 0.98f, 0.98f, 0.53f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.69f, 0.69f, 0.69f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.59f, 0.59f, 0.59f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.49f, 0.49f, 0.49f, 1.00f };
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.24f, 0.52f, 0.88f, 1.00f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.40f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.06f, 0.53f, 0.98f, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.31f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.80f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.50f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.67f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.95f };
        colors[ImGuiCol_PlotLines] = ImVec4{ 0.39f, 0.39f, 0.39f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.35f };
    }
    else if (theme == Theme::Cherry)
    {
        ImGui::StyleColorsDark();
#define HI(v) ImVec4{ 0.502f, 0.075f, 0.256f, v }
#define MED(v) ImVec4{ 0.455f, 0.198f, 0.301f, v }
#define LOW(v) ImVec4{ 0.232f, 0.201f, 0.271f, v }
#define BG(v) ImVec4{ 0.200f, 0.220f, 0.270f, v }
#define TEXT(v) ImVec4{ 0.860f, 0.930f, 0.890f, v }

        colors[ImGuiCol_Text] = ImVec4{ 0.95f, 0.96f, 0.98f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };

        colors[ImGuiCol_WindowBg] = ImVec4{ 0.13f, 0.14f, 0.17f, 1.00f };
        colors[ImGuiCol_PopupBg] = BG(0.9f);
        colors[ImGuiCol_Border] = ImVec4{ 0.31f, 0.31f, 1.00f, 0.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = BG(1.00f);
        colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
        colors[ImGuiCol_FrameBgActive] = MED(1.00f);
        colors[ImGuiCol_TitleBg] = LOW(1.00f);
        colors[ImGuiCol_TitleBgActive] = HI(1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
        colors[ImGuiCol_MenuBarBg] = BG(0.47f);
        colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.09f, 0.15f, 0.16f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
        colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.71f, 0.22f, 0.27f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.14f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.71f, 0.22f, 0.27f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.14f };
        colors[ImGuiCol_ButtonHovered] = MED(0.86f);
        colors[ImGuiCol_ButtonActive] = MED(1.00f);
        colors[ImGuiCol_Header] = MED(0.76f);
        colors[ImGuiCol_HeaderHovered] = MED(0.86f);
        colors[ImGuiCol_HeaderActive] = HI(1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.04f };
        colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
        colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
        colors[ImGuiCol_PlotLines] = TEXT(0.63f);
        colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
        colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
        colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
        colors[ImGuiCol_Border] = ImVec4{ 0.539f, 0.479f, 0.255f, 0.162f };
        colors[ImGuiCol_TabHovered] = colors[ImGuiCol_ButtonHovered];
    }
    else if (theme == Theme::Blue)
    {
        ImVec4 textColor{ 236.f / 255.f, 240.f / 255.f, 241.f / 255.f, 1.0f };
        ImVec4 headColor{ 41.f / 255.f, 128.f / 255.f, 185.f / 255.f, 1.0f };
        ImVec4 areaColor{ 57.f / 255.f, 79.f / 255.f, 105.f / 255.f, 1.0f };
        ImVec4 bodyColor{ 44.f / 255.f, 62.f / 255.f, 80.f / 255.f, 1.0f };
        ImVec4 popsColor{ 33.f / 255.f, 46.f / 255.f, 60.f / 255.f, 1.0f };

        colors[ImGuiCol_Text] = ImVec4{ textColor.x, textColor.y, textColor.z, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ textColor.x, textColor.y, textColor.z, 0.58f };

        colors[ImGuiCol_WindowBg] = ImVec4{ bodyColor.x, bodyColor.y, bodyColor.z, 0.95f };
        colors[ImGuiCol_Border] = ImVec4{ bodyColor.x, bodyColor.y, bodyColor.z, 0.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ bodyColor.x, bodyColor.y, bodyColor.z, 0.00f };
        colors[ImGuiCol_FrameBg] = ImVec4{ areaColor.x, areaColor.y, areaColor.z, 1.00f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.78f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_TitleBg] = ImVec4{ areaColor.x, areaColor.y, areaColor.z, 1.00f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ areaColor.x, areaColor.y, areaColor.z, 0.75f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ areaColor.x, areaColor.y, areaColor.z, 0.47f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ areaColor.x, areaColor.y, areaColor.z, 1.00f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.21f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.78f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_CheckMark] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.80f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.50f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.50f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.86f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.76f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.86f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.15f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.78f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_PlotLines] = ImVec4{ textColor.x, textColor.y, textColor.z, 0.63f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ textColor.x, textColor.y, textColor.z, 0.63f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ headColor.x, headColor.y, headColor.z, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ headColor.x, headColor.y, headColor.z, 0.43f };
        colors[ImGuiCol_PopupBg] = ImVec4{ popsColor.x, popsColor.y, popsColor.z, 0.92f };
    }
    else if (theme == Theme::Classic)
    {
        ImGui::StyleColorsClassic();
    }
    else if (theme == Theme::ClassicDark)
    {
        ImGui::StyleColorsDark();
    }
    else if (theme == Theme::ClassicLight)
    {
        ImGui::StyleColorsLight();
    }
    else if (theme == Theme::Cinder)
    {
        colors[ImGuiCol_Text] = ImVec4{ 0.95f, 0.96f, 0.98f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.13f, 0.14f, 0.17f, 1.00f };
        colors[ImGuiCol_Border] = ImVec4{ 0.31f, 0.31f, 1.00f, 0.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.20f, 0.22f, 0.27f, 1.00f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.78f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.86f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.20f, 0.22f, 0.27f, 0.75f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.86f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.20f, 0.22f, 0.27f, 1.00f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.09f, 0.15f, 0.16f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.78f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.71f, 0.22f, 0.27f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.14f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.14f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.86f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.76f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.86f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.47f, 0.77f, 0.83f, 0.04f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.78f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_PlotLines] = ImVec4{ 0.86f, 0.93f, 0.89f, 0.63f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.86f, 0.93f, 0.89f, 0.63f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 0.92f, 0.18f, 0.29f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.92f, 0.18f, 0.29f, 0.43f };
        colors[ImGuiCol_PopupBg] = ImVec4{ 0.20f, 0.22f, 0.27f, 0.9f };
    }
    else if (theme == Theme::Dracula)
    {
        ImGui::StyleColorsDark();

        ImVec4 Titlebar = ImVec4{ 36.0f / 255.0f, 38.0f / 255.0f, 48.0f / 255.0f, 1.0f };
        ImVec4 TabActive = ImVec4{ 40.0f / 255.0f, 42.0f / 255.0f, 54.0f / 255.0f, 1.0f };
        ImVec4 TabUnactive = ImVec4{ 35.0f / 255.0f, 43.0f / 255.0f, 59.0f / 255.0f, 1.0f };

        colors[ImGuiCol_Text] = ImVec4{ 159.0f / 255.0f, 159.0f / 255.0f, 163.0f / 255.0f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };

        colors[ImGuiCol_WindowBg] = TabActive;
        colors[ImGuiCol_ChildBg] = TabActive;

        colors[ImGuiCol_PopupBg] = ImVec4{ 42.0f / 255.0f, 38.0f / 255.0f, 47.0f / 255.0f, 1.00f };
        colors[ImGuiCol_Border] = ImVec4{ 0.08f, 0.10f, 0.12f, 1.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
        colors[ImGuiCol_FrameBg] = ImVec4{ 65.0f / 255.0f, 79.0f / 255.0f, 92.0f / 255.0f, 1.00f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.12f, 0.20f, 0.28f, 1.00f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.09f, 0.12f, 0.14f, 1.00f };

        colors[ImGuiCol_TitleBg] = Titlebar;
        colors[ImGuiCol_TitleBgActive] = Titlebar;
        colors[ImGuiCol_TitleBgCollapsed] = Titlebar;
        colors[ImGuiCol_MenuBarBg] = Titlebar;

        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.02f, 0.02f, 0.02f, 0.39f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.6f, 0.6f, 0.6f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.7f, 0.7f, 0.7f, 1.00f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.8f, 0.8f, 0.8f, 1.00f };

        colors[ImGuiCol_CheckMark] = ImVec4{ 155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ 155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 185.0f / 255.0f, 160.0f / 255.0f, 237.0f / 255.0f, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 59.0f / 255.0f, 46.0f / 255.0f, 80.0f / 255.0f, 1.0f };
        colors[ImGuiCol_ButtonActive] = colors[ImGuiCol_ButtonHovered] + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };

        colors[ImGuiCol_Separator] = ImVec4{ 0.20f, 0.25f, 0.29f, 1.00f };
        colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.10f, 0.40f, 0.75f, 0.78f };
        colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.10f, 0.40f, 0.75f, 1.00f };

        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.25f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.67f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.95f };

        colors[ImGuiCol_PlotLines] = ImVec4{ 0.61f, 0.61f, 0.61f, 1.00f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.26f, 0.59f, 0.98f, 0.35f };
        colors[ImGuiCol_DragDropTarget] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_NavHighlight] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.20f };
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.35f };

        colors[ImGuiCol_Header] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_HeaderHovered] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_HeaderActive] = TabActive + ImVec4{ 0.05f, 0.05f, 0.05f, 0.1f };

#ifdef IMGUI_HAS_DOCK

        colors[ImGuiCol_Tab] = TabUnactive;
        colors[ImGuiCol_TabHovered] = TabActive + ImVec4{ 0.1f, 0.1f, 0.1f, 0.1f };
        colors[ImGuiCol_TabActive] = TabActive;
        colors[ImGuiCol_TabUnfocused] = TabUnactive;
        colors[ImGuiCol_TabUnfocusedActive] = TabActive;
        colors[ImGuiCol_DockingEmptyBg] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };
        colors[ImGuiCol_DockingPreview] = ImVec4{ 0.33f, 0.33f, 0.33f, 1.00f };

#endif
    } else if (theme == Theme::Steam) {
        colors[ImGuiCol_Text] = ImVec4{1.00f, 1.00f, 1.00f, 1.00f};
        colors[ImGuiCol_TextDisabled] = ImVec4{0.50f, 0.50f, 0.50f, 1.00f};
        colors[ImGuiCol_WindowBg] = ImVec4{0.29f, 0.34f, 0.26f, 1.00f};
        colors[ImGuiCol_ChildBg] = ImVec4{0.29f, 0.34f, 0.26f, 1.00f};
        colors[ImGuiCol_PopupBg] = ImVec4{0.24f, 0.27f, 0.20f, 1.00f};
        colors[ImGuiCol_Border] = ImVec4{0.54f, 0.57f, 0.51f, 0.50f};
        colors[ImGuiCol_BorderShadow] = ImVec4{0.14f, 0.16f, 0.11f, 0.52f};
        colors[ImGuiCol_FrameBg] = ImVec4{0.24f, 0.27f, 0.20f, 1.00f};
        colors[ImGuiCol_FrameBgHovered] = ImVec4{0.27f, 0.30f, 0.23f, 1.00f};
        colors[ImGuiCol_FrameBgActive] = ImVec4{0.30f, 0.34f, 0.26f, 1.00f};
        colors[ImGuiCol_TitleBg] = ImVec4{0.24f, 0.27f, 0.20f, 1.00f};
        colors[ImGuiCol_TitleBgActive] = ImVec4{0.29f, 0.34f, 0.26f, 1.00f};
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.00f, 0.00f, 0.00f, 0.51f};
        colors[ImGuiCol_MenuBarBg] = ImVec4{0.24f, 0.27f, 0.20f, 1.00f};
        colors[ImGuiCol_ScrollbarBg] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.28f, 0.32f, 0.24f, 1.00f};
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.25f, 0.30f, 0.22f, 1.00f};
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.23f, 0.27f, 0.21f, 1.00f};
        colors[ImGuiCol_CheckMark] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_SliderGrab] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_SliderGrabActive] = ImVec4{0.54f, 0.57f, 0.51f, 0.50f};
        colors[ImGuiCol_Button] = ImVec4{0.29f, 0.34f, 0.26f, 0.40f};
        colors[ImGuiCol_ButtonHovered] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_ButtonActive] = ImVec4{0.54f, 0.57f, 0.51f, 0.50f};
        colors[ImGuiCol_Header] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_HeaderHovered] = ImVec4{0.35f, 0.42f, 0.31f, 0.6f};
        colors[ImGuiCol_HeaderActive] = ImVec4{0.54f, 0.57f, 0.51f, 0.50f};
        colors[ImGuiCol_Separator] = ImVec4{0.14f, 0.16f, 0.11f, 1.00f};
        colors[ImGuiCol_SeparatorHovered] = ImVec4{0.54f, 0.57f, 0.51f, 1.00f};
        colors[ImGuiCol_SeparatorActive] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_ResizeGrip] = ImVec4{0.19f, 0.23f, 0.18f, 0.00f}; // grip invis
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.54f, 0.57f, 0.51f, 1.00f};
        colors[ImGuiCol_ResizeGripActive] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_Tab] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_TabHovered] = ImVec4{0.54f, 0.57f, 0.51f, 0.78f};
        colors[ImGuiCol_TabActive] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_TabUnfocused] = ImVec4{0.24f, 0.27f, 0.20f, 1.00f};
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.35f, 0.42f, 0.31f, 1.00f};
        colors[ImGuiCol_DockingPreview] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_DockingEmptyBg] = ImVec4{0.20f, 0.20f, 0.20f, 1.00f};
        colors[ImGuiCol_PlotLines] = ImVec4{0.61f, 0.61f, 0.61f, 1.00f};
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_PlotHistogram] = ImVec4{1.00f, 0.78f, 0.28f, 1.00f};
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{1.00f, 0.60f, 0.00f, 1.00f};
        colors[ImGuiCol_TextSelectedBg] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_DragDropTarget] = ImVec4{0.73f, 0.67f, 0.24f, 1.00f};
        colors[ImGuiCol_NavHighlight] = ImVec4{0.59f, 0.54f, 0.18f, 1.00f};
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4{1.00f, 1.00f, 1.00f, 0.70f};
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4{0.80f, 0.80f, 0.80f, 0.20f};
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4{0.80f, 0.80f, 0.80f, 0.35f};

        style.FrameBorderSize = 1.0f;
        style.WindowRounding = 0.0f;
        style.ChildRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.PopupRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.TabRounding = 0.0f;
    }

    colors[ImGuiCol_Separator] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_Separator];
    colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_Separator];

    colors[ImGuiCol_Tab] = colors[ImGuiCol_MenuBarBg];
    colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_MenuBarBg];

    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_TabActive] = colors[ImGuiCol_WindowBg];
    colors[ImGuiCol_ChildBg] = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_TabActive];

    //colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
    //colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_MenuBarBg] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg] + ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f };

    colors[ImGuiCol_Border] = ImVec4{ 0.08f, 0.10f, 0.12f, 0.00f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
}
}