#include "content_browser_panel.hpp"
#include "fusion/utils/files.hpp"

#include <imgui/imgui.h>

using namespace fe;

void ContentBrowserPanel::onImGui() {
    ImGui::Begin("Content Browser");

    if (currentDirectory != getAssetPath()) {
        if (ImGui::Button(fs::ICON_FA_REPLY)) {
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

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto& entry : fs::walk(currentDirectory)) {
        const auto& path = entry.path();
        auto filename = path.filename();

        ImGui::PushID(filename.c_str());
        if (path == currentFile)
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.3f, 0.3f, 0.3f, 1.0f });
        else
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.15f, 0.15f, 0.15f, 1.0f });

        if (entry.is_directory()) {
            ImGui::Button(fs::ICON_FA_FOLDER, { thumbnailSize, thumbnailSize });
        } else {
            auto key { fs::extentions.find(filename.extension()) };
            ImGui::Button(key != fs::extentions.end() ? key->second.c_str() : fs::ICON_FA_FILE, { thumbnailSize, thumbnailSize });
        }

        if (ImGui::BeginDragDropSource()) {
            const auto& pathStr = path.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (entry.is_directory())
                    currentDirectory /= filename;
            } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                currentFile = path;
            }
        }

        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    //ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}