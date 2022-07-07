#include "imgui_utils.hpp"

#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/images/image2d_array.hpp"
#include "fusion/graphics/images/image_cube.hpp"

#include "fusion/utils/string.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/imgui/material_design_icons.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace ImGuiUtils {
using namespace fe;
using namespace std::string_literals;

glm::vec4 SelectedColor = glm::vec4{0.28f, 0.56f, 0.9f, 1.0f};
glm::vec4 IconColor = glm::vec4{0.2f, 0.2f, 0.2f, 1.0f};

bool Property(const std::string& name, std::string& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::TextUnformatted(value.c_str());
    } else {
        // TODO
    }

    Tooltip(value.c_str());

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

void PropertyConst(const std::string& name, const std::string& value) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    {
        ImGui::TextUnformatted(value.c_str());
    }

    Tooltip(value.c_str());
    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

bool Property(const std::string& name, bool& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::TextUnformatted(value ? "True" : "False");
    } else {
        std::string id = "##" + name;
        if (ImGui::Checkbox(id.c_str(), &value))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, int& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%i", value);
    } else {
        std::string id = "##" + name;
        if (ImGui::DragInt(id.c_str(), &value))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, uint32_t& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%uui", value);
    } else {
        std::string id = "##" + name;
        int valueInt = static_cast<int>(value);
        if (ImGui::DragInt(id.c_str(), &valueInt)) {
            updated = true;
            value = (uint32_t) valueInt;
        }
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, float& value, float min, float max, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f", value);
    } else {
        std::string id = "##" + name;
        if (ImGui::DragFloat(id.c_str(), &value, min, max))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, double& value, double min, double max, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f", static_cast<float>(value));
    } else {
        std::string id = "##" + name;
        if (ImGui::DragScalar(id.c_str(), ImGuiDataType_Double, &value))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, int& value, int min, int max, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%i", value);
    } else {
        std::string id = "##" + name;
        if (ImGui::DragInt(id.c_str(), &value, 1, min, max))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, glm::vec2& value, const bitmask::bitmask<PropertyFlag>& flags) {
    return Property(name, value, -1.0f, 1.0f, flags);
}

bool Property(const std::string& name, glm::vec2& value, float min, float max, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f , %.2f", value.x, value.y);
    } else {
        std::string id = "##" + name;
        if (ImGui::DragFloat2(id.c_str(), glm::value_ptr(value)))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, glm::vec3& value, const bitmask::bitmask<PropertyFlag>& flags) {
    return Property(name, value, -1.0f, 1.0f, flags);
}

bool Property(const std::string& name, glm::vec3& value, float min, float max, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f , %.2f, %.2f", value.x, value.y, value.z);
    } else {
        std::string id = "##" + name;
        if (flags & PropertyFlag::ColorProperty) {
            if (ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
                updated = true;
        } else {
            if (ImGui::DragFloat3(id.c_str(), glm::value_ptr(value)))
                updated = true;
        }
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, glm::vec4& value, bool exposeW, const bitmask::bitmask<PropertyFlag>& flags) {
    return Property(name, value, -1.0f, 1.0f, exposeW, flags);
}

bool Property(const std::string& name, glm::vec4& value, float min, float max, bool exposeW, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f , %.2f, %.2f , %.2f", value.x, value.y, value.z, value.w);
    } else {
        std::string id = "##" + name;
        if (flags & PropertyFlag::ColorProperty) {
            if (ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
                updated = true;
        } else if (exposeW && ImGui::DragFloat4(id.c_str(), glm::value_ptr(value)))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool Property(const std::string& name, glm::quat& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);
    if (flags & PropertyFlag::ReadOnly) {
        ImGui::Text("%.2f , %.2f, %.2f , %.2f", value.x, value.y, value.z, value.w);
    } else {
        std::string id = "##" + name;
        if (ImGui::DragFloat4(id.c_str(), glm::value_ptr(value)))
            updated = true;
    }

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool PropertyControl(const std::string& name, glm::vec3& value, float min, float max, float reset, float speed) {
    uint8_t updated = 0;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::PushID(name.c_str());

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("X", buttonSize)) {
            value.x = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##X", &value.x, speed, min, max, "%.2f");
    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("Y", buttonSize)) {
            value.y = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##Y", &value.y, speed, min, max, "%.2f");
    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("Z", buttonSize)) {
            value.z = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##Z", &value.z, speed, min, max, "%.2f");

    ImGui::PopStyleVar();

    ImGui::PopID();

    return updated;
}

bool PropertyControl(const std::string& name, glm::vec2& value, float min, float max, float reset, float speed) {
    uint8_t updated = 0;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::PushID(name.c_str());

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("X", buttonSize)) {
            value.x = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##X", &value.x, speed, min, max, "%.2f");
    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("Y", buttonSize)) {
            value.y = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##Y", &value.y, speed, min, max, "%.2f");

    ImGui::PopStyleVar();

    ImGui::PopID();

    return updated;
}

bool PropertyControl(const std::string& name, glm::quat& value, float min, float max, float reset, float speed) {
    uint8_t updated = 0;

    ImGuiIO& io = ImGui::GetIO();

    ImGui::PushID(name.c_str());

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("X", buttonSize)) {
            value.x = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##X", &value.x, speed, min, max, "%.2f");
    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("Y", buttonSize)) {
            value.y = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##Y", &value.y, speed, min, max, "%.2f");
    ImGui::SameLine();

    {

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("Z", buttonSize)) {
            value.z = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##Z", &value.z, speed, min, max, "%.2f");
    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.2f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.3f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.2f, 0.8f, 1.0f });
        ImGui::PushFont(io.Fonts->Fonts[1]);
        if (ImGui::Button("W", buttonSize)) {
            value.x = reset;
            updated += 1;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    updated += ImGui::DragFloat("##W", &value.x, speed, min, max, "%.2f");

    ImGui::PopStyleVar();

    ImGui::PopID();

    return updated;
}

template<typename E>
bool PropertyDropdown(const std::string& name, E& value, const bitmask::bitmask<PropertyFlag>& flags) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    constexpr auto entries = me::enum_entries<E>();
    const char* current = entries[me::enum_index(value).value_or(0)].second.data();

    if (flags & PropertyFlag::ReadOnly) {
        ImGui::TextUnformatted(current);
    } else {
        std::string id = "##" + name;
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

bool PropertyDropdown(const std::string& name, std::string* options, int32_t optionCount, int32_t* selected) {
    bool updated = false;

    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    const char* current = options[*selected].c_str();

    std::string id = "##" + name;
    if (ImGui::BeginCombo(id.c_str(), current)) {
        for (int i = 0; i < optionCount; i++) {
            const bool is_selected = (current == options[i]);
            if (ImGui::Selectable(options[i].c_str(), is_selected)) {
                current = options[i].c_str();
                *selected = i;
                updated = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    DrawItemActivityOutline(2.5f);

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return updated;
}

bool PropertyFile(const std::string& name, const std::filesystem::path& path, std::filesystem::path& value, std::vector<std::filesystem::path>& files, std::filesystem::path& selected, ImGuiTextFilter& filter) {
    bool updated = false;

    if (files.empty()) {
        files = FileSystem::GetFiles("", true);
        //LOG_WARNING << "Folder seems to be empty!";
    }

    //ImGui::Columns(2);
    ImGui::TextUnformatted(name.c_str());
    ImGui::NextColumn();

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{4, 4});

    if (ImGui::BeginPopup("FileExplorer")) {
        ImGui::TextUnformatted(ICON_MDI_FILE_SEARCH);
        ImGui::SameLine();

        ImGuiIO& io = ImGui::GetIO();

        {
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
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
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, ImGui::GetStyle().FramePadding.y});
            ImGui::TextUnformatted("Search...");
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }

        ImGui::Separator();

        ImGui::BeginChild("FileBrowser", ImVec2{ 300.0f, 500.0f });

        for (const auto& file : files) {
            if (filter.IsActive() && !filter.PassFilter(file.c_str()))
                continue;

            std::string title = FileSystem::GetIcon(file) + " "s + file.filename().string();
            if (ImGui::Selectable(title.c_str(), selected == file, ImGuiSelectableFlags_AllowDoubleClick)) {
                selected = file;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    value = file;
                    updated = true;
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::EndChild();
        ImGui::Separator();

        if (!selected.empty()) {
            std::string title = FileSystem::GetIcon(selected) + " "s + selected.string();
            ImGui::TextUnformatted(title.c_str());
        }

        ImGui::EndPopup();
    }

    ImVec2 buttonSize{ ImGui::GetContentRegionAvail().x - lineHeight, lineHeight };

    if (value.empty()) {
        if (ImGui::Button("...", buttonSize)) {
            filter.Clear();
            selected = "";
            files = FileSystem::GetFiles("", true);
            ImGui::OpenPopup("FileExplorer");
        }
    } else {
        std::string title = FileSystem::GetIcon(value) + " "s + value.filename().string();
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
            value = reinterpret_cast<const char*>(payload->Data);
            updated = true;
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    if (ImGui::Button(ICON_MDI_FILE_SEARCH)) {
        filter.Clear();
        selected = "";
        files = FileSystem::GetFiles("", true);
        ImGui::OpenPopup("FileExplorer");
    }

    ImGui::PopStyleVar();

    //ImGui::Columns(1);

    return updated;
}

void Tooltip(const std::string& text) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});
    Tooltip(text.c_str());
    ImGui::PopStyleVar();
}

void Tooltip(const char* text) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(text);
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Image2d* texture, const glm::vec2& size) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Image2d* texture, const glm::vec2& size, const std::string& text) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
        ImGui::TextUnformatted(text.c_str());
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Tooltip(Image2dArray* texture, uint32_t index, const glm::vec2& size) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{5, 5});

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();

        ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
        ImGui::EndTooltip();
    }

    ImGui::PopStyleVar();
}

void Image(Image2d* texture, const glm::vec2& size) {
    ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
}

void Image(ImageCube* texture, const glm::vec2& size) {
    ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
}

void Image(Image2dArray* texture, uint32_t index, const glm::vec2& size) {
    ImGui::Image(texture, ImVec2{size.x, size.y}, ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
}

bool BufferingBar(const std::string& name, float value, const glm::vec2& size_arg, uint32_t bg_col, uint32_t fg_col) {
    auto g = ImGui::GetCurrentContext();
    auto drawList = ImGui::GetWindowDrawList();
    const ImGuiStyle& style = ImGui::GetStyle();
    const ImGuiID id = ImGui::GetID(name.c_str());

    ImVec2 pos = ImGui::GetCursorPos();
    ImVec2 size = { size_arg.x, size_arg.y };
    size.x -= style.FramePadding.x * 2;

    const ImRect bb(pos, ImVec2{pos.x + size.x, pos.y + size.y});
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    const float circleStart = size.x * 0.7f;
    const float circleEnd = size.x;
    const float circleWidth = circleEnd - circleStart;

    drawList->AddRectFilled(bb.Min, ImVec2{pos.x + circleStart, bb.Max.y}, bg_col);
    drawList->AddRectFilled(bb.Min, ImVec2{pos.x + circleStart * value, bb.Max.y}, fg_col);

    const float t = static_cast<float>(g->Time);
    const float r = size.y * 0.5f;
    const float speed = 1.5f;

    const float a = speed * 0.f;
    const float b = speed * 0.333f;
    const float c = speed * 0.666f;

    const float o1 = (circleWidth + r) * (t + a - speed * std::round((t + a) / speed)) / speed;
    const float o2 = (circleWidth + r) * (t + b - speed * std::round((t + b) / speed)) / speed;
    const float o3 = (circleWidth + r) * (t + c - speed * std::round((t + c) / speed)) / speed;

    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o1, bb.Min.y + r}, r, bg_col);
    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o2, bb.Min.y + r}, r, bg_col);
    drawList->AddCircleFilled(ImVec2{pos.x + circleEnd - o3, bb.Min.y + r}, r, bg_col);

    return true;
}

bool Spinner(const std::string& name, float radius, int thickness, uint32_t color) {
    auto g = ImGui::GetCurrentContext();
    const ImGuiStyle& style = g->Style;
    const ImGuiID id = ImGui::GetID(name.c_str());
    auto drawList = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorPos();
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2{pos.x + size.x, pos.y + size.y});
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    drawList->PathClear();

    int num_segments = 30;
    float start = abs(ImSin(static_cast<float>(g->Time) * 1.8f) * static_cast<float>(num_segments - 5));

    const float a_min = IM_PI * 2.0f * (start / static_cast<float>(num_segments));
    const float a_max = IM_PI * 2.0f * (static_cast<float>(num_segments) - 3.0f) / static_cast<float>(num_segments);

    const ImVec2 centre = ImVec2{pos.x + radius, pos.y + radius + style.FramePadding.y};

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + (static_cast<float>(i) / static_cast<float>(num_segments)) * (a_max - a_min);
        drawList->PathLineTo(ImVec2{centre.x + ImCos(a + static_cast<float>(g->Time) * 8) * radius,
                                    centre.y + ImSin(a + static_cast<float>(g->Time) * 8) * radius});
    }

    drawList->PathStroke(color, false, static_cast<float>(thickness));

    return true;
}

void DrawRowsBackground(int row_count, float line_height, float x1, float x2, float y_offset, uint32_t col_even, uint32_t col_odd) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float y0 = ImGui::GetCursorScreenPos().y + std::round(y_offset);

    ImGuiListClipper clipper;
    clipper.Begin(row_count, line_height);
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            ImU32 col = (i & 1) ? col_odd : col_even;
            if ((col & IM_COL32_A_MASK) == 0)
                continue;
            float y1 = y0 + (line_height * static_cast<float>(i));
            float y2 = y1 + line_height;
            drawList->AddRectFilled(ImVec2{ x1, y1 }, ImVec2{ x2, y2 }, col);
        }
    }
}

const glm::vec4& GetSelectedColor() {
    return SelectedColor;
}

const glm::vec4& GetIconColor() {
    return IconColor;
}

void DrawItemActivityOutline(float rounding, bool drawWhenInactive, const ImColor& colourWhenActive) {
    auto* drawList = ImGui::GetWindowDrawList();

    ImRect expandedRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    expandedRect.Min.x -= 1.0f;
    expandedRect.Min.y -= 1.0f;
    expandedRect.Max.x += 1.0f;
    expandedRect.Max.y += 1.0f;

    const ImRect rect = expandedRect;
    if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max,
                          ImColor{60, 60, 60}, rounding, 0, 1.5f);
    }
    if (ImGui::IsItemActive()) {
        drawList->AddRect(rect.Min, rect.Max,
                          colourWhenActive, rounding, 0, 1.0f);
    } else if (!ImGui::IsItemHovered() && drawWhenInactive) {
        drawList->AddRect(rect.Min, rect.Max,
                          ImColor{50, 50, 50}, rounding, 0, 1.0f);
    }
}

bool InputText(std::string& currentText) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

    char buffer[256];
    memset(buffer, 0, 256);
    memcpy(buffer, currentText.c_str(), currentText.length());

    bool updated = ImGui::InputText("##SceneName", buffer, 256);

    DrawItemActivityOutline(2.0f, false);

    if (updated)
        currentText = std::string{buffer};

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    return updated;
}

// from https://github.com/ocornut/imgui/issues/2668
void AlternatingRowsBackground(float lineHeight) {
    const ImU32 im_color = ImGui::GetColorU32(ImGuiCol_TableRowBgAlt);

    auto* drawList = ImGui::GetWindowDrawList();
    const auto& style = ImGui::GetStyle();

    if (lineHeight < 0) {
        lineHeight = ImGui::GetTextLineHeight();
    }

    lineHeight += style.ItemSpacing.y;

    float scroll_offset_h = ImGui::GetScrollX();
    float scroll_offset_v = ImGui::GetScrollY();
    float scrolled_out_lines = std::floor(scroll_offset_v / lineHeight);
    scroll_offset_v -= lineHeight * scrolled_out_lines;

    ImVec2 clip_rect_min(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    ImVec2 clip_rect_max(clip_rect_min.x + ImGui::GetWindowWidth(), clip_rect_min.y + ImGui::GetWindowHeight());

    if (ImGui::GetScrollMaxX() > 0) {
        clip_rect_max.y -= style.ScrollbarSize;
    }

    drawList->PushClipRect(clip_rect_min, clip_rect_max);

    const float y_min = clip_rect_min.y - scroll_offset_v + ImGui::GetCursorPosY();
    const float y_max = clip_rect_max.y - scroll_offset_v + lineHeight;
    const float x_min = clip_rect_min.x + scroll_offset_h + ImGui::GetWindowContentRegionMin().x;
    const float x_max = clip_rect_min.x + scroll_offset_h + ImGui::GetWindowContentRegionMax().x;

    bool is_odd = (static_cast<int>(scrolled_out_lines) % 2) == 0;
    for (float y = y_min; y < y_max; y += lineHeight, is_odd = !is_odd) {
        if (is_odd) {
            drawList->AddRectFilled({ x_min, y - style.ItemSpacing.y }, { x_max, y + lineHeight }, im_color);
        }
    }

    drawList->PopClipRect();
}

void SetTheme(Theme theme) {
    static const float max = 255.0f;

    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    SelectedColor = glm::vec4{ 0.28f, 0.56f, 0.9f, 1.0f };

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
        ImVec4 Titlebar = ImVec4{ 40.0f / max, 42.0f / max, 54.0f / max, 1.0f };
        ImVec4 TabActive = ImVec4{ 52.0f / max, 54.0f / max, 64.0f / max, 1.0f };
        ImVec4 TabUnactive = ImVec4{ 35.0f / max, 43.0f / max, 59.0f / max, 1.0f };

        SelectedColor = ImVec4{ 155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f };
        colors[ImGuiCol_Text] = ImVec4{ 200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };

        IconColor = colors[ImGuiCol_Text];
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
        IconColor = colors[ImGuiCol_Text];

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
        IconColor = colors[ImGuiCol_Text];

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
#define TEXTCol(v) ImVec4{ 0.860f, 0.930f, 0.890f, v }

        colors[ImGuiCol_Text] = ImVec4{ 0.95f, 0.96f, 0.98f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };
        IconColor = colors[ImGuiCol_Text];

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
        colors[ImGuiCol_PlotLines] = TEXTCol(0.63f);
        colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
        colors[ImGuiCol_PlotHistogram] = TEXTCol(0.63f);
        colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
        colors[ImGuiCol_Border] = ImVec4{ 0.539f, 0.479f, 0.255f, 0.162f };
        colors[ImGuiCol_TabHovered] = colors[ImGuiCol_ButtonHovered];
    }
    else if (theme == Theme::Blue)
    {
        ImVec4 color_for_text = ImVec4{ 236.f / 255.f, 240.f / 255.f, 241.f / 255.f, 1.0f };
        ImVec4 color_for_head = ImVec4{ 41.f / 255.f, 128.f / 255.f, 185.f / 255.f, 1.0f };
        ImVec4 color_for_area = ImVec4{ 57.f / 255.f, 79.f / 255.f, 105.f / 255.f, 1.0f };
        ImVec4 color_for_body = ImVec4{ 44.f / 255.f, 62.f / 255.f, 80.f / 255.f, 1.0f };
        ImVec4 color_for_pops = ImVec4{ 33.f / 255.f, 46.f / 255.f, 60.f / 255.f, 1.0f };
        colors[ImGuiCol_Text] = ImVec4{ color_for_text.x, color_for_text.y, color_for_text.z, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ color_for_text.x, color_for_text.y, color_for_text.z, 0.58f };
        IconColor = colors[ImGuiCol_Text];

        colors[ImGuiCol_WindowBg] = ImVec4{ color_for_body.x, color_for_body.y, color_for_body.z, 0.95f };
        colors[ImGuiCol_Border] = ImVec4{ color_for_body.x, color_for_body.y, color_for_body.z, 0.00f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ color_for_body.x, color_for_body.y, color_for_body.z, 0.00f };
        colors[ImGuiCol_FrameBg] = ImVec4{ color_for_area.x, color_for_area.y, color_for_area.z, 1.00f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.78f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_TitleBg] = ImVec4{ color_for_area.x, color_for_area.y, color_for_area.z, 1.00f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ color_for_area.x, color_for_area.y, color_for_area.z, 0.75f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ color_for_area.x, color_for_area.y, color_for_area.z, 0.47f };
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ color_for_area.x, color_for_area.y, color_for_area.z, 1.00f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.21f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.78f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_CheckMark] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.80f };
        colors[ImGuiCol_SliderGrab] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.50f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_Button] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.50f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.86f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_Header] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.76f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.86f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_ResizeGrip] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.15f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.78f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_PlotLines] = ImVec4{ color_for_text.x, color_for_text.y, color_for_text.z, 0.63f };
        colors[ImGuiCol_PlotLinesHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_PlotHistogram] = ImVec4{ color_for_text.x, color_for_text.y, color_for_text.z, 0.63f };
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 1.00f };
        colors[ImGuiCol_TextSelectedBg] = ImVec4{ color_for_head.x, color_for_head.y, color_for_head.z, 0.43f };
        colors[ImGuiCol_PopupBg] = ImVec4{ color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f };
    }
    else if (theme == Theme::Classic)
    {
        ImGui::StyleColorsClassic();
        IconColor = colors[ImGuiCol_Text];
    }
    else if (theme == Theme::ClassicDark)
    {
        ImGui::StyleColorsDark();
        IconColor = colors[ImGuiCol_Text];
    }
    else if (theme == Theme::ClassicLight)
    {
        ImGui::StyleColorsLight();
        IconColor = colors[ImGuiCol_Text];
    }
    else if (theme == Theme::Cinder)
    {
        colors[ImGuiCol_Text] = ImVec4{ 0.95f, 0.96f, 0.98f, 1.00f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.36f, 0.42f, 0.47f, 1.00f };
        IconColor = colors[ImGuiCol_Text];
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

        ImVec4 Titlebar = ImVec4{ 36.0f / max, 38.0f / max, 48.0f / max, 1.0f };
        ImVec4 TabActive = ImVec4{ 40.0f / max, 42.0f / max, 54.0f / max, 1.0f };
        ImVec4 TabUnactive = ImVec4{ 35.0f / max, 43.0f / max, 59.0f / max, 1.0f };

        IconColor = ImVec4{ 183.0f / 255.0f, 158.0f / 255.0f, 220.0f / 255.0f, 1.00f };
        SelectedColor = ImVec4{ 145.0f / 255.0f, 111.0f / 255.0f, 186.0f / 255.0f, 1.00f };
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

    colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_MenuBarBg] = colors[ImGuiCol_TitleBg];
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_WindowBg] + ImVec4{ 0.05f, 0.05f, 0.05f, 0.0f };

    colors[ImGuiCol_Border] = ImVec4{ 0.08f, 0.10f, 0.12f, 0.00f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
}
}