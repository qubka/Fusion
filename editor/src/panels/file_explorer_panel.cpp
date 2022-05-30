#include "file_explorer_panel.hpp"
#include "fusion/utils/files.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace fe;
using namespace std::string_literals;

void FileExplorerPanel::onImGui() {
    ImGui::Begin("##fileexplorer", nullptr, ImGuiWindowFlags_NoTitleBar);

    std::function<void(const std::filesystem::path&, uint32_t &)> function = [&](const std::filesystem::path& dir, uint32_t& idx) {
        for (const auto& entry : fs::walk(dir)) {
            const auto& path = entry.path();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((currentNode == idx) ? ImGuiTreeNodeFlags_Selected : 0);

            if (entry.is_directory()) {
                bool opened = ImGui::TreeNodeEx((void *)static_cast<intptr_t>(idx), flags, "");

                ImGui::SameLine();
                ImGui::Text("%s %s", opened ? fs::ICON_FA_FOLDER_OPEN : fs::ICON_FA_FOLDER_CLOSE, path.filename().c_str());

                if (opened) {
                    if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
                        currentNode = idx;
                    function(entry, ++idx);
                    ImGui::TreePop();
                }
            } else {
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                ImGui::TreeNodeEx((void *)static_cast<intptr_t>(idx), flags, "%s %s", fs::extension_icon(path).c_str(), path.filename().c_str());
                if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
                    currentNode = idx;

                if (ImGui::BeginDragDropSource()) {
                    const auto& pathStr = path.string();
                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
                    ImGui::EndDragDropSource();
                }
            }

            ++idx;
        }
    };

    uint32_t idx = 0;
    function(getAssetPath(), idx);

    ImGui::End();
}
