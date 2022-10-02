#include "content_browser_panel.hpp"

#include "editor.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/filesystem/file_format.hpp"

#include <imgui/imgui.h>

using namespace fe;

ContentBrowserPanel::ContentBrowserPanel(Editor& editor) : EditorPanel{ICON_MDI_ARCHIVE " Content Browser###content", "ContentBrowser", editor} {
    root = editor.getProjectSettings().projectRoot / "assets";

    auto base = processDirectory(root, nullptr);
    baseDirectory = directories[base];
    changeDirectory(baseDirectory);
}

ContentBrowserPanel::~ContentBrowserPanel() {

}

void ContentBrowserPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin(title.c_str(), &active, flags);
    {
        ImVec2 windowSize{ ImGui::GetWindowSize() };
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
            ImGui::BeginChild("##directory_breadcrumbs", ImVec2{ImGui::GetColumnWidth(), ImGui::GetFrameHeightWithSpacing()});
            {
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

            ImGui::BeginChild("##Scrolling");
            {
                int shownIndex = 0;

                float xAvail = ImGui::GetContentRegionAvail().x;

                gridItemsPerRow = glm::max(1, static_cast<int>(glm::floor(xAvail / (gridSize + style.ItemSpacing.x))));

                if (isInListView) {
                    for (const auto& [i, child] : enumerate(currentDirectory->children)) {
                        if (filter.IsActive()) {
                            if (!filter.PassFilter(child->name.c_str())) {
                                continue;
                            }
                        }

                        if (drawFile(i, child->isDirectory, shownIndex, false))
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

                        if (drawFile(i, child->isDirectory, shownIndex, true))
                            break;

                        shownIndex++;
                    }
                }

                if (ImGui::BeginPopupContextWindow()) {
                    if (ImGui::Selectable("Import New Asset")) {
                        editor.openFile();
                    }

                    if (ImGui::Selectable("Refresh")) {
                        refresh();
                    }

                    if (ImGui::Selectable("New folder")) {
                        fs::create_directory(root / currentDirectory->path / "New Folder");
                        refresh();
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

    const ImU32 treeLineColor = IM_COL32(128, 128, 128, 128);
    const float smallOffsetX = 6.0f;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (dirInfo->isDirectory) {
        bool isContains = false;

        for (const auto& file: dirInfo->children) {
            if (file->isDirectory) {
                isContains = true;
                break;
            }
        }

        if (!isContains)
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;

        if (defaultOpen)
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

        bool isOpened = ImGui::TreeNodeEx((void*)(intptr_t)dirInfo.get(), nodeFlags, "");

        if (ImGui::IsItemClicked() && !isLocked) {
            changeDirectory(dirInfo);
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                fs::path file = fs::current_path() / fs::path{static_cast<const char*>(payload->Data)};
                fs::path move = fs::current_path() / (dirInfo->parent ? root / dirInfo->path : root);
                if (moveFile(file, move)) {
                    LOG_INFO << "Moved File: \"" << file << "\" to \"" << move << "\"";
                    refresh();
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (dirInfo->parent != nullptr)
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::TextUnformatted(dirInfo->icon);

                ImGui::SameLine();

                std::string pathStr{ (root / dirInfo->path).string() };
                ImGui::TextUnformatted(pathStr.c_str());
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
                ImGui::EndDragDropSource();
            }

        const char* folderIcon = ((isOpened && isContains) || currentDirectory == dirInfo) ? ICON_MDI_FOLDER_OPEN : ICON_MDI_FOLDER;
        ImGui::SameLine();
        //ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorScheme::GetIconColor());
        ImGui::Text("%s ", folderIcon);
        //ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextUnformatted(dirInfo->name.c_str());

        if (isOpened) {
            ImVec2 verticalLineStart{ ImGui::GetCursorScreenPos() };
            verticalLineStart.x += smallOffsetX; // to nicely line up with the arrow symbol
            ImVec2 verticalLineEnd{ verticalLineStart };

            for (const auto& file : dirInfo->children) {
                if (file->isDirectory) {
                    ImVec2 currentPos{ ImGui::GetCursorScreenPos() };

                    ImGui::Indent(10.0f);

                    bool containsFolderTemp = false;
                    for (const auto& f : file->children) {
                        if (f->isDirectory) {
                            containsFolderTemp = true;
                            break;
                        }
                    }

                    float horizontalTreeLineSize = 16.0f;
                    if (containsFolderTemp)
                        horizontalTreeLineSize *= 0.5f;

                    drawFolder(file);

                    ImRect childRect{currentPos, currentPos + ImVec2{0.0f, ImGui::GetFontSize()}};

                    const float midpoint = (childRect.Min.y + childRect.Max.y) * 0.5f;
                    drawList->AddLine(ImVec2{verticalLineStart.x, midpoint},
                                      ImVec2{verticalLineStart.x + horizontalTreeLineSize, midpoint},
                                      treeLineColor);
                    verticalLineEnd.y = midpoint;

                    ImGui::Unindent(10.0f);
                }
            }

            drawList->AddLine(verticalLineStart, verticalLineEnd, treeLineColor);

            ImGui::TreePop();
        }
    }
}

bool ContentBrowserPanel::drawFile(size_t dirIndex, bool folder, int shownIndex, bool gridView) {
    const auto& directory = currentDirectory->children[dirIndex];
    const auto& [parent, children, path, name, icon, dir] = *directory;

    bool isDoubleClicked = false;

    if (gridView) {
        ImGui::PushID(shownIndex);

        ImGui::BeginGroup();

        if (ImGui::Button(folder ? ICON_MDI_FOLDER : icon, ImVec2{gridSize, gridSize})) {
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            isDoubleClicked = true;
        }

        ImGui::TextUnformatted(name.c_str());
        ImGui::EndGroup();

        ImGui::PopID();

        if ((shownIndex + 1) % gridItemsPerRow != 0)
            ImGui::SameLine();
    } else {
        ImGui::TextUnformatted(folder ? ICON_MDI_FOLDER : icon);
        ImGui::SameLine();
        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                isDoubleClicked = true;
            }
        }
    }

    ImGuiUtils::Tooltip(name.c_str());

    if (isDoubleClicked) {
        if (folder) {
            changeDirectory(directory);
            return true;
        } else {
            editor.fileOpenCallback(root / path);
        }
    }

    if (folder)
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                fs::path file = fs::current_path() / fs::path{static_cast<const char*>(payload->Data)};
                fs::path move = fs::current_path() / (parent ? root / path : root);
                if (moveFile(file, move)) {
                    LOG_INFO << "Moved File: \"" << file << "\" to \"" << move << "\"";
                    refresh();
                    return true;
                }
            }
            ImGui::EndDragDropTarget();
        }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::TextUnformatted(icon);

        ImGui::SameLine();

        std::string pathStr{ (root / path).string() };
        ImGui::TextUnformatted(pathStr.c_str());
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathStr.c_str(), pathStr.length() + 1);
        ImGui::EndDragDropSource();
    }

    return isDoubleClicked;
}

void ContentBrowserPanel::drawBottom() {
    const float sizeWidth = 150.0f;

    ImGui::BeginChild("##nav", ImVec2{ImGui::GetColumnWidth() - 10.0f, ImGui::GetFontSize() * 1.8f}, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        float xAvail = ImGui::GetContentRegionAvail().x;

        ImVec4 color{0.1f, 0.2f, 0.7f, 0.0f};
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));

        for (const auto& directory : breadCrumbData) {
            if (ImGui::SmallButton(directory->name.c_str())) {
                if (!isLocked) {
                    changeDirectory(directory);
                }
            }

            ImGui::SameLine();
            ImGui::TextUnformatted(" / ");
            ImGui::SameLine();
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine(xAvail > sizeWidth * static_cast<float>(breadCrumbData.size()) ? xAvail - sizeWidth : 0.0f);
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
    root = editor.getProjectSettings().projectRoot / "assets";

    const auto& currentPath = currentDirectory->path;

    updateNavigationPath = true;

    directories.clear();
    auto base = processDirectory(root, nullptr);
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

const fs::path& ContentBrowserPanel::processDirectory(const fs::path& path, const std::shared_ptr<DirectoryInfo>& parent) {
    const auto& directory = directories[path];
    if (directory)
        return directory->path;

    auto directoryInfo = std::make_shared<DirectoryInfo>(path);
    directoryInfo->parent = parent;

    if (/*path == root*/parent == nullptr)
        directoryInfo->path = root;
    else
        directoryInfo->path = fs::relative(path, root);

    if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            const auto& subdir = processDirectory(entry.path(), directoryInfo);
            directoryInfo->children.push_back(directories[subdir]);
        }
    }

    directories[directoryInfo->path] = directoryInfo;
    return directoryInfo->path;
}

bool ContentBrowserPanel::moveFile(const fs::path& filepath, const fs::path& movepath) {
    std::string cmd{ String::Quoted(filepath.string()) + " " + String::Quoted(movepath.string()) };
#if FUSION_PLATFORM_LINUX
    system(("mv " + cmd).c_str());
#else
    #ifndef FUSION_PLATFORM_IOS
        system(("move " + cmd).c_str());
    #endif
#endif
    return fs::exists(movepath / filepath.filename());
}

DirectoryInfo::DirectoryInfo(const fs::path& filepath) {
    name = filepath.filename().string();
    icon = FileFormat::GetIcon(filepath);
    isDirectory = fs::is_directory(filepath);
}
