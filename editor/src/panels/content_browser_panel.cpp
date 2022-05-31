#include "content_browser_panel.hpp"
#include "fusion/utils/files.hpp"

#include <imgui/imgui.h>

using namespace fe;
using namespace std::string_literals;

void ContentBrowserPanel::onImGui() {
    ImGui::Begin((fs::ICON_FA_ARCHIVE + "  Content Browser"s).c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    drawFileExplorer();
    drawContentBrowser();

    ImGui::End();
}

void ContentBrowserPanel::drawFileExplorer() {
    ImGui::BeginChild((fs::ICON_FA_ARCHIVE + "  Project"s).c_str(), { ImGui::GetContentRegionAvail().x / 6.0f, 0 }, true);

    std::function<void(const std::filesystem::path&, uint32_t &)> function = [&](const std::filesystem::path& dir, uint32_t& idx) {
        for (const auto& entry : fs::walk(dir)) {
            const auto& path = entry.path();

            if (entry.is_directory()) {
                bool filled = fs::has_directories(path);

                ImGuiTreeNodeFlags flags = ((currentNode == idx) ? ImGuiTreeNodeFlags_Selected : 0) |
                                           (filled ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick) : ImGuiTreeNodeFlags_Leaf)
                                           | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

                bool opened = ImGui::TreeNodeEx((void *)static_cast<intptr_t>(idx), flags, "");
                if (ImGui::IsItemClicked() || ImGui::IsItemFocused()) {
                    if (!locked) currentDirectory = path;
                    currentNode = idx;
                }

                ImGui::SameLine();
                ImGui::Text("%s %s", opened && filled ? fs::ICON_FA_FOLDER_OPEN : fs::ICON_FA_FOLDER_CLOSE, path.filename().c_str());

                if (opened) {
                    function(entry, ++idx);
                    ImGui::TreePop();
                }

                ++idx;
            }
        }
    };

    uint32_t idx = 0;
    function(getAssetPath(), idx);

    ImGui::EndChild();
}

void ContentBrowserPanel::drawContentBrowser() {
    ImGui::SameLine();
    ImGui::BeginChild("ContentBrowser", { 0, 0 }, true);

    if (currentDirectory != getAssetPath()) {
        if (ImGui::Button(fs::ICON_FA_REPLY)) {
            currentDirectory = currentDirectory.parent_path();
        }
        ImGui::SameLine();
    }

    ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
    ImGui::SameLine();

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, filter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##filesfilter", "Search Files", buffer, sizeof(buffer))) {
        filter = std::string{buffer};
    }

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16.0f);
    if (ImGui::Button(locked ? fs::ICON_FA_LOCK : fs::ICON_FA_UNLOCK)) {
        locked = !locked;
    }

    ImGui::Separator();

    static float padding = 16.0f;
    static float thumbnailSize = 64.0f;
    static float cellSize = thumbnailSize + padding;

    int columnCount = static_cast<int>(ImGui::GetContentRegionAvail().x / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto& entry : (filter.empty() ? fs::walk(currentDirectory) : fs::recursive_walk(getAssetPath(), filter))) {
        const auto& path = entry.path();

        ImGui::PushID(path.c_str());

        if (path == currentFile)
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.3f, 0.3f, 0.3f, 1.0f });
        else
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.15f, 0.15f, 0.15f, 1.0f });

        if (entry.is_directory()) {
            ImGui::Button(fs::ICON_FA_FOLDER_CLOSE, { thumbnailSize, thumbnailSize });
        } else {
            ImGui::Button(fs::extension_icon(path).c_str(), { thumbnailSize, thumbnailSize });
        }

        if (ImGui::BeginDragDropSource()) {
            const auto& pathStr = path.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor(3);

        auto filename = path.filename();

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

    ImGui::EndChild();
}

/*std::function<void(const std::filesystem::path&, uint32_t &)> function = [&](const std::filesystem::path& dir, uint32_t& idx) {
    for (const auto& entry : (folderFilter.empty() ? fs::walk(dir) : fs::recursive_walk(dir, folderFilter))) {
        const auto& path = entry.path();

        ImGuiTreeNodeFlags flags = ((currentNode == idx) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

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
};*/