#include "content_browser_panel.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

    std::function<void(const std::filesystem::path&, uint64_t &)> function = [&](const std::filesystem::path& dir, uint64_t& idx) {
        for (const auto& file : fs::walk(dir)) {
            if (is_directory(file)) {
                bool filled = fs::has_directories(file);

                ImGuiTreeNodeFlags flags = ((currentNode == idx) ? ImGuiTreeNodeFlags_Selected : 0) |
                                           (filled ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick) : ImGuiTreeNodeFlags_Leaf)
                                           | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

                bool opened = ImGui::TreeNodeEx((void *)idx, flags, "");
                if (ImGui::IsItemClicked() || ImGui::IsItemFocused()) {
                    if (!locked) selectDirectory(file);
                    currentNode = idx;
                }

                ImGui::SameLine();
                ImGui::Text("%s %s", opened && filled ? fs::ICON_FA_FOLDER_OPEN : fs::ICON_FA_FOLDER_CLOSE, file.filename().c_str());

                if (opened) {
                    function(file, ++idx);
                    ImGui::TreePop();
                }

                ++idx;
            }
        }
    };

    uint64_t idx = 0;
    function(getAssetPath(), idx);

    ImGui::EndChild();
}

void ContentBrowserPanel::drawContentBrowser() {
    ImGui::SameLine();
    ImGui::BeginChild("ContentBrowser", { 0, 0 }, true);

    if (currentDirectory != getAssetPath()) {
        if (ImGui::Button(fs::ICON_FA_REPLY))
            selectDirectory(currentDirectory.parent_path());
        ImGui::SameLine();
    }

    ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
    ImGui::SameLine();

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, filter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##filesfilter", "Search Files", buffer, sizeof(buffer))) {
        filter = std::string{buffer};
        cachedFiles = fs::recursive_walk(getAssetPath(), filter);
    }

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->FontSize);
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

    bool updateFiles = false;

    for (const auto& file : (filter.empty() ? currentFiles : cachedFiles)) {
        ImGui::PushID(file.c_str());

        if (currentFile == file)
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.3f, 0.3f, 0.3f, 1.0f });
        else
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.15f, 0.15f, 0.15f, 1.0f });

        if (is_directory(file)) {
            ImGui::Button(fs::ICON_FA_FOLDER_CLOSE, { thumbnailSize, thumbnailSize });
        } else {
            ImGui::Button(fs::extension_icon(file).c_str(), { thumbnailSize, thumbnailSize });
        }

        if (ImGui::BeginDragDropSource()) {
            const auto& fileStr = file.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", fileStr.c_str(), fileStr.length() + 1);
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor(3);

        auto filename = file.filename();

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (is_directory(file)) {
                    currentDirectory /= filename;
                    updateFiles = true;
                }
            } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                currentFile = file;
            }
        }

        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    if (updateFiles)
        currentFiles = fs::walk(currentDirectory);

    ImGui::Columns(1);

    //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
    //ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::EndChild();
}

void ContentBrowserPanel::selectFile(const std::filesystem::path& file) {
    if (!locked) selectDirectory(file.parent_path());
    currentFile = file;
}

void ContentBrowserPanel::selectDirectory(const std::filesystem::path& dir) {
    currentDirectory = dir;
    currentFiles = fs::walk(dir);
}

/*std::function<void(const std::filesystem::path&, uint32_t &)> function = [&](const std::filesystem::path& dir, uint32_t& idx) {
    for (const auto& entry : (folderFilter.empty() ? fs::walk(dir) : fs::recursive_walk(dir, folderFilter))) {
        const auto& path = entry.path();

        ImGuiTreeNodeFlags flags = ((currentNode == idx) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

        if (entry.is_directory()) {
            bool opened = ImGui::TreeNodeEx((void *)idx, flags, "");

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