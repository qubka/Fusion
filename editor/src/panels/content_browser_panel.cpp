#include "content_browser_panel.hpp"

#include <imgui/imgui.h>

using namespace fe;

ContentBrowserPanel::ContentBrowserPanel() : currentDirectory{getAssetPath()} {

}

void ContentBrowserPanel::onImGui() {
    ImGui::Begin("Content Browser");

    if (currentDirectory.string() != getAssetPath()) {
        if (ImGui::Button("\uF112")) {
            currentDirectory = currentDirectory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnailSize = 64.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory)) {
        const auto& path = directoryEntry.path();
        auto relativePath = std::filesystem::relative(path, getAssetPath());
        auto filename = relativePath.filename();

        ImGui::PushID(filename.c_str());
        ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});

        if (directoryEntry.is_directory()) {
            ImGui::Button("\uF07C", { thumbnailSize, thumbnailSize });
        } else {
            auto key { extentions.find(filename.extension()) };
            ImGui::Button(key != extentions.end() ? key->second.c_str() : "\uF016", { thumbnailSize, thumbnailSize });
        }

        if (ImGui::BeginDragDropSource()) {
            auto itemStr = relativePath.wstring();
            const wchar_t* itemPath = itemStr.c_str();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered()) {
            if (directoryEntry.is_directory()) {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    currentDirectory /= path.filename();
            } else {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    currentFile = directoryEntry;
            }
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
