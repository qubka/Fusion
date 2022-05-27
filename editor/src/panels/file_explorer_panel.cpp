#include "file_explorer_panel.hpp"
#include "fusion/utils/files.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace fe;

void FileExplorerPanel::onImGui() {
    ImGui::Begin("File Explorer");

    static int selectMask = (1 << 2);
    uint32_t clicked = UINT32_MAX;
    
    std::function<void(const std::filesystem::path&, uint32_t &)> function = [&](const std::filesystem::path& dir, uint32_t& idx) {
        for (const auto& entry : fs::walk(dir)) {
            const auto& path = entry.path();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selectMask & (1 << idx)) ? ImGuiTreeNodeFlags_Selected : 0);

            if (entry.is_directory()) {
                if (ImGui::TreeNodeEx((void *)static_cast<intptr_t>(idx), flags, "%s %s", fs::ICON_FA_FOLDER, path.filename().c_str())) {
                    if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
                        clicked = idx;
                    function(entry, ++idx);
                    ImGui::TreePop();
                }
            } else {
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                auto filename = path.filename();
                auto key { fs::extentions.find(filename.extension()) };
                ImGui::TreeNodeEx((void *)static_cast<intptr_t>(idx), flags, "%s %s", key != fs::extentions.end() ? key->second.c_str() : fs::ICON_FA_FILE, filename.c_str());
                if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
                    clicked = idx;
            }

            if (ImGui::BeginDragDropSource()) {
                const auto& pathStr = path.string();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
                ImGui::EndDragDropSource();
            }

            ++idx;
        }
    };

    uint32_t idx = 0;
    function(getAssetPath(), idx);
    if (clicked != UINT32_MAX) {
        // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
        if (ImGui::GetIO().KeyCtrl)
            selectMask ^= (1 << clicked);          // CTRL+click to toggle
        else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
            selectMask = (1 << clicked);           // Click to single-select
    }

    ImGui::End();
}
