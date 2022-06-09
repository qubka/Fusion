#include "content_browser_panel.hpp"

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

    std::function<void(const std::filesystem::path&)> function = [&](const std::filesystem::path& dir) {
        for (const auto& file : fs::walk(dir)) {
            if (is_directory(file)) {
                bool filled = fs::has_directories(file);

                ImGuiTreeNodeFlags flags = ((currentDirectory == file) ? ImGuiTreeNodeFlags_Selected : 0) |
                                           (filled ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick) : ImGuiTreeNodeFlags_Leaf)
                                           | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

                bool opened = ImGui::TreeNodeEx(file.c_str(), flags, "");
                if (ImGui::IsItemClicked() || ImGui::IsItemFocused()) {
                    selectDirectory(file);
                }

                ImGui::SameLine();
                ImGui::Text("%s %s", opened && filled ? fs::ICON_FA_FOLDER_OPEN : fs::ICON_FA_FOLDER_CLOSE, file.filename().c_str());

                if (opened) {
                    function(file);
                    ImGui::TreePop();
                }
            }
        }
    };

    function(getAssetPath());

    ImGui::EndChild();
}

void ContentBrowserPanel::drawContentBrowser() {
    ImGui::SameLine();
    ImGui::BeginChild("ContentPanel", { 0, 0 }, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (currentDirectory != getAssetPath()) {
        if (ImGui::Button(fs::ICON_FA_REPLY))
            selectDirectory(currentDirectory.parent_path());
        ImGui::SameLine();
    }

    ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
    ImGui::SameLine();

    static char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, filter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##filesfilter", "Search Files", buffer, sizeof(buffer))) {
        filter = std::string{buffer};
        cachedFiles = fs::recursive_walk(getAssetPath(), filter);
    }

    ImGui::Separator();

    ImVec2 contentRegionAvail{ ImGui::GetContentRegionAvail() };
    ImGui::BeginChild("ContentBrowser", { 0, contentRegionAvail.y - ImGui::GetFontSize() * 1.5f });

    static int padding = 16;
    float thumbnailSize = padding * 4.0f;
    float cellSize = thumbnailSize + padding;

    int columnCount = static_cast<int>(contentRegionAvail.x / cellSize);
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

        ImGui::TextUnformatted(filename.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    if (updateFiles)
        currentFiles = fs::walk(currentDirectory);

    ImGui::Columns(1);

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        currentFile = "";
    }

    ImGui::EndChild();
    ImGui::Separator();

    if (!currentFile.empty()) {
        std::string title{ fs::extension_icon(currentFile) + " " + currentFile.string() };
        ImGui::TextUnformatted(title.c_str());
    }

    ImGui::SameLine(contentRegionAvail.x - 150.0f);

    ImGui::PushItemWidth(150.0f);
    ImGui::SliderInt("##padding", &padding, 16, 64);

    ImGui::NewLine();

    ImGui::EndChild();
}

void ContentBrowserPanel::selectFile(const std::filesystem::path& file) {
    selectDirectory(file.parent_path());
    currentFile = file;
}

void ContentBrowserPanel::selectDirectory(const std::filesystem::path& dir) {
    currentDirectory = dir;
    currentFiles = fs::walk(dir);
}