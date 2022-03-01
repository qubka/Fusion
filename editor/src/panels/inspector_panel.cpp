#include "inspector_panel.hpp"

#include <imgui/imgui.h>
#include <vulkan/vulkan.hpp>

using namespace fe;

void InspectorPanel::onImGui() {
    // * texture */

    ImGui::Begin("Inspector");

    const char* types[] = { "2D", "2DArray", "CubeMap" };

    // This is not very useful (may obsolete): prefer using BeginCombo()/EndCombo() for full control.
    static int type_current_idx = 0; // If the selection isn't within 0..count, Combo won't display a preview
    ImGui::Combo("Texture Type", &type_current_idx, types, IM_ARRAYSIZE(types));

    constexpr auto format_entries = magic_enum::enum_values<vk::Format>();

    static int format_current_idx = magic_enum::enum_integer(vk::Format::eR8G8B8A8Unorm); // Here we store our selection data as an index.
    if (ImGui::BeginCombo("Format", to_string(format_entries[format_current_idx]).c_str()))
    {
        for (int n = 0; n < format_entries.size(); n++)
        {
            const bool is_selected = (format_current_idx == n);
            if (ImGui::Selectable(to_string(format_entries[n]).c_str(), is_selected))
                format_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}
