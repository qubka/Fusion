#include "content_browser_panel.hpp"

#include "editor.hpp"

#include "fusion/filesystem/file_system.hpp"
#include "fusion/utils/string.hpp"
#include "fusion/utils/enumerate.hpp"

#include <imgui/imgui.h>

using namespace fe;

ContentBrowserPanel::ContentBrowserPanel() : EditorPanel{ICON_MDI_ARCHIVE " Content Browser##content", "ContentBrowser"} {
    basePath = std::filesystem::current_path(); // TODO: Finish project system
    auto base = processDirectory(basePath, nullptr);
    baseDirectory = directories[base];
    changeDirectory(baseDirectory);
}

ContentBrowserPanel::~ContentBrowserPanel() {

}

void ContentBrowserPanel::onImGui() {
    ImGui::Begin(name.c_str(), &enabled);
    {
        auto windowSize = ImGui::GetWindowSize();
        bool vertical = windowSize.y > windowSize.x;

        if (!vertical) {
            ImGui::BeginColumns("ResourcePanelColumns", 2, ImGuiOldColumnFlags_NoResize);
            ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 3.0f);
            ImGui::BeginChild("##folders_common");
        } else
            ImGui::BeginChild("##folders_common", ImVec2{0, ImGui::GetWindowHeight() / 3.0f});

        {
            //drawBreadCrumbs();

            {
                ImGui::BeginChild("##folders");
                {
                    drawFolder(baseDirectory, true);
                }
                ImGui::EndChild();
            }
        }

        ImGui::EndChild();

        float offset = 0.0f;
        if (!vertical) {
            ImGui::NextColumn();
        } else {
            offset = ImGui::GetWindowHeight() / 3.0f + 6.0f;
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
        }

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::BeginChild("##directory_structure", ImVec2{0, ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() * 2.6f - offset});
        {
            {
                ImGui::BeginChild("##directory_breadcrumbs", ImVec2{ImGui::GetColumnWidth(), ImGui::GetFrameHeightWithSpacing()});

                if (ImGui::Button(ICON_MDI_ARROW_LEFT)) {
                    if (currentDirectory != baseDirectory && !isLocked) {
                        changeDirectory(currentDirectory->parent);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(ICON_MDI_ARROW_RIGHT)) {
                    if (!isLocked) {
                        //changeDirectory(lastNavPath); // TODO: Implement
                    }
                }
                ImGui::SameLine();

                if (updateNavigationPath) {
                    breadCrumbData.clear();
                    auto current = currentDirectory;
                    while (current) {
                        if (current->parent != nullptr) {
                            breadCrumbData.push_back(current);
                            current = current->parent;
                        } else {
                            breadCrumbData.push_back(baseDirectory);
                            current = nullptr;
                        }
                    }

                    std::reverse(breadCrumbData.begin(), breadCrumbData.end());
                    updateNavigationPath = false;
                }

                if (isInListView) {
                    if (ImGui::Button(ICON_MDI_VIEW_GRID)) {
                        isInListView = !isInListView;
                    }
                    ImGui::SameLine();
                } else {
                    if (ImGui::Button(ICON_MDI_VIEW_LIST)) {
                        isInListView = !isInListView;
                    }
                    ImGui::SameLine();
                }

                ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
                ImGui::SameLine();

                filter.Draw("##Filter", ImGui::GetContentRegionAvail().x - style.IndentSpacing - ImGui::GetFontSize() * 1.5f);

                ImGui::SameLine();
                if (ImGui::Button(isLocked ? ICON_MDI_LOCK : ICON_MDI_LOCK_OPEN)) {
                    isLocked = !isLocked;
                }

                ImGui::EndChild();
            }

            {
                ImGui::BeginChild("##Scrolling");

                int shownIndex = 0;

                float xAvail = ImGui::GetContentRegionAvail().x;

                gridItemsPerRow = static_cast<int>(std::floor(xAvail / (gridSize + style.ItemSpacing.x)));
                gridItemsPerRow = std::max(1, gridItemsPerRow);

                if (isInListView) {
                    for (const auto& [i, child] : enumerate(currentDirectory->children)) {
                        if (filter.IsActive()) {
                            if (!filter.PassFilter(child->name.c_str())) {
                                continue;
                            }
                        }

                        bool doubleClicked = drawFile(i, child->isDirectory, shownIndex, !isInListView);
                        if (doubleClicked)
                            break;

                        shownIndex++;
                    }
                } else {
                    for (const auto& [i, child] : enumerate(currentDirectory->children)) {
                        if (filter.IsActive()) {
                            if (!filter.PassFilter(child->name.c_str())) {
                                continue;
                            }
                        }

                        bool doubleClicked = drawFile(i, child->isDirectory, shownIndex, !isInListView);
                        if (doubleClicked)
                            break;

                        shownIndex++;
                    }
                }

                if (ImGui::BeginPopupContextWindow()) {
                    {
                        if (ImGui::Selectable("Import New Asset")) {
                            //editor->openFile();
                        }

                        if (ImGui::Selectable("Refresh")) {
                            refresh();
                        }

                        if (ImGui::Selectable("New folder")) {
                            std::filesystem::create_directory(basePath / currentDirectory->path / "New Folder");
                            refresh();
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndChild();
            }

            ImGui::EndChild();

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            drawBottom();
        }

        ImGui::End();
    }
}

void ContentBrowserPanel::drawFolder(const std::shared_ptr<DirectoryInfo>& dirInfo, bool defaultOpen) {
    ImGuiTreeNodeFlags nodeFlags = ((dirInfo == currentDirectory) ? ImGuiTreeNodeFlags_Selected : 0);
    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (dirInfo->parent == nullptr)
        nodeFlags |= ImGuiTreeNodeFlags_Framed;

    const ImColor TreeLineColor = ImColor{128, 128, 128, 128};
    const float smallOffsetX = 6.0f;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (dirInfo->isDirectory) {
        bool isContains = false;

        for (auto& file: dirInfo->children) {
            if (file->isDirectory) {
                isContains = true;
                break;
            }
        }

        if (!isContains)
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;

        if (defaultOpen)
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool isOpened = ImGui::TreeNodeEx(reinterpret_cast<void*>(dirInfo.get()), nodeFlags, "");

        const char* folderIcon = ((isOpened && isContains) || currentDirectory == dirInfo) ? ICON_MDI_FOLDER_OPEN : ICON_MDI_FOLDER;
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtils::GetIconColor());
        ImGui::Text("%s ", folderIcon);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(dirInfo->name.c_str());

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::filesystem::path path{ reinterpret_cast<const char*>(payload->Data) };
                /*if (canMove(path, movePath)) {
                    LOG_INFO << "Moved File: " << path << " to " << movePath;
                }*/
                LOG_INFO << path;
                isDragging = false;
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::TextUnformatted(dirInfo->icon);

            ImGui::SameLine();
            movePath = basePath / dirInfo->path;
            ImGui::TextUnformatted(movePath.c_str());
            size_t size = sizeof(const char*) + strlen(movePath.c_str());
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", movePath.c_str(), size);
            isDragging = true;
            ImGui::EndDragDropSource();
        }

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

        if (ImGui::IsItemClicked() && !isLocked) {
            changeDirectory(dirInfo);
        }

        if (isOpened) {
            verticalLineStart.x += smallOffsetX; // to nicely line up with the arrow symbol
            ImVec2 verticalLineEnd = verticalLineStart;

            for (auto& file : dirInfo->children) {
                if (file->isDirectory) {
                    auto currentPos = ImGui::GetCursorScreenPos();

                    ImGui::Indent(10.0f);

                    bool containsFolderTemp = false;
                    for (auto& f : file->children) {
                        if (f->isDirectory) {
                            containsFolderTemp = true;
                            break;
                        }
                    }

                    float horizontalTreeLineSize = 16.0f;
                    if (containsFolderTemp)
                        horizontalTreeLineSize *= 0.5f;

                    drawFolder(file);

                    const ImRect childRect = ImRect{currentPos, currentPos + ImVec2{0.0f, ImGui::GetFontSize()}};

                    const float midpoint = (childRect.Min.y + childRect.Max.y) * 0.5f;
                    drawList->AddLine(ImVec2{verticalLineStart.x, midpoint},
                                      ImVec2{verticalLineStart.x + horizontalTreeLineSize, midpoint},
                                      TreeLineColor);
                    verticalLineEnd.y = midpoint;

                    ImGui::Unindent(10.0f);
                }
            }

            drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

            ImGui::TreePop();
        }
    }

    if (isDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
        movePath = dirInfo->path.string().c_str();
    }
}

bool ContentBrowserPanel::drawFile(size_t dirIndex, bool folder, int shownIndex, bool gridView) {
    const auto& directory = currentDirectory->children[dirIndex];
    const auto& [parent, children, path, name, icon, dir] = *directory;

    //auto fileID = GetParsedAssetID(FileSystem::GetExtension(path));

    bool doubleClicked = false;

    if (gridView) {
        ImGui::BeginGroup();

        if (ImGui::Button(folder ? ICON_MDI_FOLDER : icon, ImVec2{gridSize, gridSize})) {
        }

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            doubleClicked = true;
        }

        ImGui::TextUnformatted(name.c_str());
        ImGui::EndGroup();

        if ((shownIndex + 1) % gridItemsPerRow != 0)
            ImGui::SameLine();
    } else {
        ImGui::TextUnformatted(folder ? ICON_MDI_FOLDER : icon);
        ImGui::SameLine();
        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                doubleClicked = true;
            }
        }
    }

    ImGuiUtils::Tooltip(name);

    if (doubleClicked) {
        if (folder) {
            changeDirectory(directory);
            return true;
        } else {
            editor->fileOpenCallback(basePath / path);
        }
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::TextUnformatted(icon);

        ImGui::SameLine();
        movePath = basePath / path;
        ImGui::TextUnformatted(movePath.c_str());
        size_t size = sizeof(const char*) + strlen(movePath.c_str());
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", movePath.c_str(), size);
        isDragging = true;
        ImGui::EndDragDropSource();
    }

    return doubleClicked;
}

void ContentBrowserPanel::drawBottom() {
    const float sizeWidth = 150.0f;

    ImGui::BeginChild("##nav", ImVec2{ImGui::GetColumnWidth() - 10.0f, ImGui::GetFontSize() * 1.8f}, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        float xAvail = ImGui::GetContentRegionAvail().x;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.2f, 0.7f, 0.0f});

        for (auto& directory : breadCrumbData) {
            if (ImGui::SmallButton(directory->name.c_str())) {
                if (!isLocked) {
                    changeDirectory(directory);
                }
            }

            ImGui::SameLine();
            ImGui::TextUnformatted(" / ");
            ImGui::SameLine();
        }

        ImGui::PopStyleColor();

        ImGui::SameLine(xAvail - sizeWidth);
    }

    if (!isInListView) {
        ImGui::PushItemWidth(sizeWidth);
        ImGui::SliderFloat("##GridSize", &gridSize, 40.0f, 400.0f);
        ImGui::NewLine();
    }

    ImGui::EndChild();
}

void ContentBrowserPanel::onNewProject() {
    refresh();
}

void ContentBrowserPanel::refresh() {
    basePath = std::filesystem::current_path(); // TODO: Finish project system

    const auto& currentPath = currentDirectory->path;

    updateNavigationPath = true;

    directories.clear();
    auto base = processDirectory(basePath, nullptr);
    baseDirectory = directories[base];
    previousDirectory = nullptr;
    currentDirectory = nullptr;

    if (directories.find(currentPath) != directories.end())
        currentDirectory = directories[currentPath];
    else
        changeDirectory(baseDirectory);
}

void ContentBrowserPanel::changeDirectory(const std::shared_ptr<DirectoryInfo>& directory) {
    if(!directory)
        return;

    previousDirectory = currentDirectory;
    currentDirectory = directory;
    updateNavigationPath = true;
}

const std::filesystem::path& ContentBrowserPanel::processDirectory(const std::filesystem::path& path, const std::shared_ptr<DirectoryInfo>& parent) {
    const auto& directory = directories[path];
    if (directory)
        return directory->path;

    auto directoryInfo = std::make_shared<DirectoryInfo>(path);
    directoryInfo->parent = parent;

    if (path == basePath)
        directoryInfo->path = basePath;
    else
        directoryInfo->path = std::filesystem::relative(path, basePath);

    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            const auto& subdir = processDirectory(entry.path(), directoryInfo);
            directoryInfo->children.push_back(directories[subdir]);
        }
    }

    directories[directoryInfo->path] = directoryInfo;
    return directoryInfo->path;
}

bool ContentBrowserPanel::canMove(const std::filesystem::path& filepath, const std::filesystem::path& movepath) {
    std::string s = "move " + filepath.string() + " " + movepath.c_str();

    // TODO: Finish file move

    return std::filesystem::exists(movepath / filepath.filename());
}

DirectoryInfo::DirectoryInfo(std::filesystem::path filepath) : path{std::move(filepath)} {
    name = this->path.filename().string();
    icon = FileSystem::GetIcon(this->path);
    isDirectory = std::filesystem::is_directory(this->path);
}
