#include "scene_hierarchy_panel.hpp"
#include "content_browser_panel.hpp"

#include "fusion/scene/components.hpp"

#include <portable-file-dialogs/portable-file-dialogs.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace fe;
using namespace std::string_literals;

void SceneHierarchyPanel::setContext(const std::shared_ptr<Scene>& scene) {
    context = scene;
    selectionContext = entt::null;
    renameContext = entt::null;
}

void SceneHierarchyPanel::onImGui() {
    ImGui::Begin((fs::ICON_FA_LIST + "  Hierarchy"s).c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    drawEntities();
    ImGui::End();

    ImGui::Begin((fs::ICON_FA_INFO + "  Inspector"s).c_str());
    if (selectionContext != entt::null) {
        drawComponents(selectionContext);
    }
    ImGui::End();
}

void SceneHierarchyPanel::drawEntities() {
    ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
    ImGui::SameLine();
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, entityFilter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##entityfilter", "Search Entities", buffer, sizeof(buffer))) {
        entityFilter = std::string{buffer};
    }

    ImGui::Separator();
    ImGui::BeginChild("EntityList");

    entt::entity removeEntity{ entt::null };
    std::function<void(entt::entity entity)> function = [&](const auto entity) {
        ImGui::PushID(("Entity" + std::to_string(static_cast<int>(entity))).c_str());

        auto& tag = *context->world.get<TagComponent>(entity);
        bool children = context->world.has_children(entity);

        ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                                   (children ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick) : ImGuiTreeNodeFlags_Leaf);

        if (entity != renameContext) {
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
        } else if (renameContext != entt::null) {
            flags |= ImGuiTreeNodeFlags_FramePadding;

            float lineHeight = ImGui::GetFontSize() / 2.0f;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { lineHeight, lineHeight });
        }
        bool opened = ImGui::TreeNodeEx((void*)static_cast<intptr_t>(entity), flags, "");
        if (ImGui::IsItemClicked() || ImGui::IsItemFocused()) {
            selectionContext = entity;
        }
        /*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            renameContext = entity;
        }*/

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("SCENE_HIERARCHY_ITEM", &entity, sizeof(entt::entity));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                context->world.assign_child(entity, *reinterpret_cast<entt::entity*>(payload->Data));
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();

        if (renameContext == entity) {
            if (!ImGui::IsWindowFocused()) {
                renameContext = entt::null;
            } else {
                if (!ImGui::IsAnyItemActive()) {
                    ImGui::SetKeyboardFocusHere();
                }
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                std::strncpy(buffer, tag.c_str(), sizeof(buffer));
                if (ImGui::InputText("##entityrename", buffer, sizeof(buffer), ImGuiInputTextFlags_AutoSelectAll)) {
                    tag = std::string{buffer};
                }
                if (ImGui::IsKeyDown(ImGuiKey_Enter)) {
                    renameContext = entt::null;
                }
            }
        } else {
            ImGui::TextUnformatted(tag.c_str());
        }

        if (ImGui::BeginPopupContextItem("EntityOptions")) {
            /*if (ImGui::MenuItem("Copy")) {
            }
            if (ImGui::MenuItem("Paste")) {
            }
            ImGui::Spacing();
            if (ImGui::MenuItem("Rename")) {
                selectionContext = entity;
                renameContext = entity;
            }
            if (ImGui::MenuItem("Duplicate")) {

            }*/
            if (ImGui::MenuItem("Delete")) {
                removeEntity = entity;
            }
            ImGui::EndPopup();
        }

        if (flags & ImGuiTreeNodeFlags_FramePadding)
            ImGui::PopStyleVar();

        ImGui::PopID();

        if (opened) {
            for (auto e : context->world.get_children(entity)) {
                function(e);
            }
            ImGui::TreePop();
        }
    };

    context->world.each([&](auto entity) {
        if (context->world.get_parent(entity) == entt::null) {
            function(entity);
        }
    });

    if (removeEntity != entt::null) {
        context->world.destroy_parent(removeEntity);
        if (selectionContext == removeEntity)
            selectionContext = entt::null;
        if (renameContext == removeEntity)
            renameContext = entt::null;
    }


    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        selectionContext = entt::null;
        renameContext = entt::null;
    }

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow("HierarchyOptions", 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            auto entity = context->world.create();

            std::string name{ "Empty Entity" };
            size_t idx = 0;
            auto view = context->world.view<const TagComponent>();
            for (auto [e, tag] : view.each()) {
                if ((*tag).find(name, 0) != std::string::npos) {
                    idx++;
                }
            }

            if (idx > 0)
                name += " (" + std::to_string(idx) + ")";

            context->world.emplace<TagComponent>(entity, name);
            context->world.emplace<TransformComponent>(entity);

            selectionContext = entity;
            renameContext = entity;
        }
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}

bool SceneHierarchyPanel::drawFileBrowser(const std::string& label, std::string& value, const std::vector<std::string>& formats, float columnWidth) {
    bool modify = false;

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0f, 4.0f });

    if (ImGui::BeginPopup("FileExplorer")) {
        ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
        ImGui::SameLine();
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, fileFilter.c_str(), sizeof(buffer));
        if (ImGui::InputTextWithHint("##filefilter", "Search File", buffer, sizeof(buffer))) {
            fileFilter = std::string{buffer};
            cachedFiles = fs::recursive_walk(getAssetPath(), fileFilter, formats);
        }

        ImGui::Separator();
        ImGui::BeginChild("FileBrowser", { 300.0f, 500.0f });

        for (const auto& file : cachedFiles) {
            std::string title{ fs::extension_icon(file) + " " + file.filename().string() };
            if (ImGui::Selectable(title.c_str(), currentFile == file, ImGuiSelectableFlags_AllowDoubleClick)) {
                currentFile = file;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    value = file;
                    modify = true;
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::EndChild();
        ImGui::Separator();

        if (!currentFile.empty()) {
            std::string title{ fs::extension_icon(currentFile) + " " + currentFile.string() };
            ImGui::TextUnformatted(title.c_str());
        }

        ImGui::EndPopup();
    }

    /*auto function = [&]() {
        std::string pattern{" "};
        for (const auto& format : formats) pattern += format + " ";
        auto filepath = pfd::open_file("Choose 3D file", value.empty() ? getAssetPath() : value, { file, pattern }, pfd::opt::none).result();
        if (!filepath.empty()) {
            // Validate that file inside working directory
            if (filepath[0].find(std::filesystem::current_path()) != std::string::npos) {
                value = std::filesystem::relative(filepath[0]);
                modify = true;
            } else {
                pfd::message("File Location", "The selected file should be inside the project directory.", pfd::choice::ok, pfd::icon::error);
            }
        }
    };*/

    ImVec2 buttonSize{ ImGui::GetContentRegionAvail().x - lineHeight, lineHeight };

    if (value.empty()) {
        if (ImGui::Button("...", buttonSize)) {
            fileFilter = "";
            currentFile = "";
            cachedFiles = fs::recursive_walk(getAssetPath(), fileFilter, formats);
            ImGui::OpenPopup("FileExplorer");
        }
    } else {
        std::filesystem::path file{ value };
        std::string title{ fs::extension_icon(file) + " " + file.filename().string() };
        if (ImGui::Button(title.c_str(), buttonSize)) {
            contentBrowserPanel.selectFile(file);
        }
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        value = "";
        modify = true;
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            std::filesystem::path file{ reinterpret_cast<const char*>(payload->Data) };
            // Validate that file format is suitable
            if (std::find(formats.begin(), formats.end(), file.extension().string()) != formats.end()) {
                value = file;
                modify = true;
            } else {
                std::string pattern{" "};
                for (const auto& format : formats) pattern += format + " ";
                pfd::message("File Format", "The selected file format should be ( " + pattern + ").", pfd::choice::ok, pfd::icon::error);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    if (ImGui::Button(fs::ICON_FA_SEARCH)) {
        fileFilter = "";
        currentFile = "";
        cachedFiles = fs::recursive_walk(getAssetPath(), fileFilter, formats);
        ImGui::OpenPopup("FileExplorer");
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

bool SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
    uint8_t modify = 0;

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
    //::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

bool SceneHierarchyPanel::drawVec2Control(const std::string& label, glm::vec2& values, float resetValue, float columnWidth) {
    uint8_t modify = 0;

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

bool SceneHierarchyPanel::drawValueControl(const std::string& label, std::function<bool()>&& function, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0f, 4.0f });

    bool modify = function();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

template<typename T>
void SceneHierarchyPanel::drawComponent(const std::string& label, entt::entity entity, std::function<void(T& comp)>&& function) {
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth |
                                     ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
    if (auto component = context->world.try_get<T>(entity)) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 4.0f });
        float lineHeight = ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y;
        ImGui::Separator();

        bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", label.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(lineHeight);

        if (ImGui::Button(fs::ICON_FA_COG)) {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (opened) {
            function(*component);
            ImGui::TreePop();
        }

        if (removeComponent)
            context->world.remove<T>(entity);
    }
}

void SceneHierarchyPanel::drawComponents(entt::entity entity) {
    auto& tag = *context->world.get<TagComponent>(entity);
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, tag.c_str(), sizeof(buffer));
    if (ImGui::InputText("##tag", buffer, sizeof(buffer))) {
        tag = std::string{buffer};
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponent");

    if (ImGui::BeginPopup("AddComponent")) {
        if (!context->world.all_of<TransformComponent>(entity)) {
            if (ImGui::MenuItem("Transform")) {
                context->world.emplace<TransformComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->world.all_of<CameraComponent>(entity)) {
            if (ImGui::MenuItem("Camera")) {
                context->world.emplace<CameraComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->world.all_of<ModelComponent>(entity)) {
            if (ImGui::MenuItem("Model")) {
                context->world.emplace<ModelComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->world.all_of<RigidbodyComponent>(entity)) {
            if (ImGui::MenuItem("Rigidbody")) {
                context->world.emplace<RigidbodyComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->world.all_of<PhysicsMaterialComponent>(entity)) {
            if (ImGui::MenuItem("Physics Material")) {
                context->world.emplace<PhysicsMaterialComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->world.all_of<BoxColliderComponent>(entity)) {
            if (ImGui::MenuItem("Box Collider")) {
                context->world.emplace<BoxColliderComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<TransformComponent>(fs::ICON_FA_AXIS + "  Transform"s, entity, [&](TransformComponent& component)
    {
        uint8_t notify = 0;
        notify += drawVec3Control("Position", component.position);
        glm::vec3 rotation{ glm::degrees(glm::eulerAngles(component.rotation)) };
        notify += drawVec3Control("Rotation", rotation);
        component.rotation = glm::radians(rotation);
        notify += drawVec3Control("Scale", component.scale, 1.0f);

        if (notify)
            context->world.patch_children<TransformComponent>(entity);
    });

    drawComponent<CameraComponent>(fs::ICON_FA_CAMERA + "  Camera"s, entity, [&](CameraComponent& component)
    {
        auto& camera = component.camera;

        drawValueControl("Is Primary", [&component] { return ImGui::Checkbox("##primary", &component.primary); });

        constexpr auto projections = magic_enum::enum_entries<SceneCamera::ProjectionType>();
        auto currentProjection = camera.getProjectionType();

        drawValueControl("Type", [&] {
            bool modify = false;
            if (ImGui::BeginCombo("##projection", projections[magic_enum::enum_integer(currentProjection)].second.data())) {
                for (const auto& [type, name]: projections) {
                    bool isSelected = currentProjection == type;
                    if (ImGui::Selectable(name.data(), isSelected)) {
                        currentProjection = type;
                        camera.setProjectionType(type);
                        modify = true;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }
            return modify;
        });

        switch (camera.getProjectionType()) {
            case SceneCamera::ProjectionType::Perspective: {
                drawValueControl("Vertical FOV", [&] {
                    float perspectiveVerticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("##verticalfOV", &perspectiveVerticalFov)) {
                        camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));
                        return true;
                    }
                    return false;
                });
                drawValueControl("Near Clip", [&] {
                    float perspectiveNear = camera.getPerspectiveNearClip();
                    if (ImGui::DragFloat("##nearclip", &perspectiveNear)) {
                        camera.setPerspectiveNearClip(perspectiveNear);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Far Clip", [&] {
                    float perspectiveFar = camera.getPerspectiveFarClip();
                    if (ImGui::DragFloat("##farclip", &perspectiveFar)) {
                        camera.setPerspectiveFarClip(perspectiveFar);
                        return true;
                    }
                    return false;
                });
                break;
            }

            case SceneCamera::ProjectionType::Orthographic: {
                drawValueControl("Size", [&] {
                    float orthoSize = camera.getOrthographicSize();
                    if (ImGui::DragFloat("##size", &orthoSize)) {
                        camera.setOrthographicSize(orthoSize);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Near Clip", [&] {
                    float orthoNear = camera.getOrthographicNearClip();
                    if (ImGui::DragFloat("##nearclip", &orthoNear)) {
                        camera.setOrthographicNearClip(orthoNear);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Far Clip", [&] {
                    float orthoFar = camera.getOrthographicFarClip();
                    if (ImGui::DragFloat("##farclip", &orthoFar)) {
                        camera.setOrthographicFarClip(orthoFar);
                        return true;
                    }
                    return false;
                });

                drawValueControl("Fixed Aspect", [&component] { return ImGui::Checkbox("##fixedaspect", &component.fixedAspectRatio); });
                break;
            }
        }
    });

    drawComponent<ModelComponent>(fs::ICON_FA_CUBE + "  Model"s, entity, [&](ModelComponent& component)
    {
        uint8_t notify = 0;
        notify += drawFileBrowser("File Path", component.path, { ".fbx", ".obj", ".dae", ".gltf", ".3ds" });
        notify += drawVec3Control("Scale", component.scale);
        notify += drawVec3Control("Center", component.center);
        notify += drawVec2Control("UV Scale", component.uvscale);

        if (notify)
            context->world.patch<ModelComponent>(entity);

        /*if (ImGui::TreeNode("Layout")) {
            constexpr auto components = magic_enum::enum_entries<vkx::model::Component>();

            if (ImGui::BeginCombo("##Layout", "")) {
                for (const auto& [type, name] : components) {
                    if (ImGui::Selectable(name.data(), false)) {
                        component.layout.push_back(magic_enum::enum_integer(type));
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear All"))
                component.layout.clear();

            for (int i = 0; i < component.layout.size(); i++) {
                auto id = component.layout[i];

                ImGui::Selectable(magic_enum::enum_name(magic_enum::enum_value<vkx::model::Component>(id)).data());

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
                    int n = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n >= 0 && n < component.layout.size()) {
                        component.layout[i] = component.layout[n];
                        component.layout[n] = id;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }

            ImGui::TreePop();
        }*/
    });

    drawComponent<RigidbodyComponent>(fs::ICON_FA_BOUNDS + "  Rigidbody"s, entity, [&](RigidbodyComponent& component)
    {
        drawValueControl("Type", [&component] {
            bool modify = false;
            constexpr auto bodytypes = magic_enum::enum_entries<RigidbodyComponent::BodyType>();
            if (ImGui::BeginCombo("##body", bodytypes[magic_enum::enum_integer(component.type)].second.data())) {
                for (const auto& [type, name] : bodytypes) {
                    bool isSelected = component.type == type;
                    if (ImGui::Selectable(name.data(), isSelected)) {
                        component.type = type;
                        modify = true;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }
            return modify;
        });

        if (component.type == RigidbodyComponent::BodyType::Dynamic) {
            drawValueControl("Mass", [&component] { return ImGui::DragFloat("##mass", &component.mass, 0.1f, 0.0f, 10000.0f, "%.2f"); });
            drawValueControl("Linear Drag", [&component] { return ImGui::DragFloat("##linear", &component.linearDrag, 0.1f, 0.0f, 10000.0f, "%.2f"); });
            drawValueControl("Angular Drag", [&component] { return ImGui::DragFloat("##angular", &component.angularDrag, 0.1f, 0.0f, 10000.0f, "%.2f"); });
            drawValueControl("Disable Gravity", [&component] { return ImGui::Checkbox("##gravity", &component.disableGravity); });
            drawValueControl("Is Kinematic", [&component] { return ImGui::Checkbox("##kinematic", &component.kinematic); });
        }
    });

    drawComponent<PhysicsMaterialComponent>(fs::ICON_FA_BOUNDS + "  Physics Material"s, entity, [&](PhysicsMaterialComponent& component)
    {
        /*drawValueControl("Density", [&component] { return ImGui::DragFloat("##density", &component.density, 0.1f, 0.0f, 0.0f, "%.2f"); });
        drawValueControl("Friction", [&component] { return ImGui::DragFloat("##friction", &component.friction, 0.1f, 0.0f, 0.0f, "%.2f"); });
        drawValueControl("Restitution", [&component] { return ImGui::DragFloat("##restitution", &component.x, 0.1f, 0.0f, 0.0f, "%.2f"); });
        drawValueControl("Restitution Threshold", [&component] { return ImGui::DragFloat("##threshold", &component.x, 0.1f, 0.0f, 0.0f, "%.2f"); });*/
    });

    drawComponent<BoxColliderComponent>(fs::ICON_FA_BOUNDS + "  Box Collider"s, entity, [&](BoxColliderComponent& component)
    {
        drawVec3Control("Center", component.center);
        drawVec3Control("Size", component.size);
        drawValueControl("Is Trigger", [&component]{ return ImGui::Checkbox("##trigger", &component.trigger); });
    });

    /*drawComponent<BoundsComponent>(fs::ICON_FA_BOUNDS + "  Bounds"s, entity, [&](BoundsComponent& component)
    {
        drawVec3Control("Min", component.min);
        drawVec3Control("Max", component.max);
    });*/
}