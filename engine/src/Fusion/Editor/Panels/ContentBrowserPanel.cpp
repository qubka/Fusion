#include "ContentBrowserPanel.hpp"

#include <imgui/imgui.h>

using namespace Fusion;

ContentBrowserPanel::ContentBrowserPanel() : currentDirectory{AssetPath} {

}

void ContentBrowserPanel::onImGui() {
    ImGui::Begin("Content Browser");

    if (currentDirectory != AssetPath) {
        if (ImGui::Button("\uF112")) {
            currentDirectory = currentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 64.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory)) {
        const auto& path = directoryEntry.path();
        auto relativePath = std::filesystem::relative(path, AssetPath);
        std::string filename = relativePath.filename().string();

        ImGui::PushID(filename.c_str());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::Button(directoryEntry.is_directory() ? "\uF07C" : "\uF15C", { thumbnailSize, thumbnailSize });

        if (ImGui::BeginDragDropSource()) {
            auto itemStr = relativePath.wstring();
            const wchar_t* itemPath = itemStr.c_str();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
            if (directoryEntry.is_directory())
                currentDirectory /= path.filename();
        }
        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}
