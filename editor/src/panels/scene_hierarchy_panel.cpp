#include "scene_hierarchy_panel.hpp"
#include "content_browser_panel.hpp"

#include "fusion/scene/components.hpp"
#include "fusion/systems/parent_system.hpp"
#include "fusion/utils/directory.hpp"
#include "fusion/utils/file.hpp"
#include "fusion/utils/string.hpp"

#include <portable-file-dialogs/portable-file-dialogs.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <IconsFontAwesome4.h>

using namespace fe;

void SceneHierarchyPanel::setContext(const std::shared_ptr<Scene>& scene) {
    context = scene;
    selectionContext = entt::null;
    renameContext = entt::null;
}

void SceneHierarchyPanel::onImGui() {
    ImGui::Begin((ICON_FA_LIST + "  Hierarchy"s).c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    drawEntities();
    ImGui::End();

    ImGui::Begin((ICON_FA_INFO_CIRCLE + "  Inspector"s).c_str());
    if (context->registry.valid(selectionContext)) {
        drawComponents(selectionContext);
    }
    ImGui::End();
}

void SceneHierarchyPanel::drawEntities() {
    ImGui::TextUnformatted(ICON_FA_SEARCH);
    ImGui::SameLine();
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, entityFilter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##entityfilter", "Search Entities", buffer, sizeof(buffer))) {
        entityFilter = std::string{buffer};
    }

    ImGui::Separator();
    ImGui::BeginChild("EntityList");

    ParentSystem parentSystem{ context->registry };

    entt::entity removeEntity = entt::null;
    std::function<void(entt::entity entity)> function = [&](const auto entity) {
        ImGui::PushID(("Entity" + std::to_string(static_cast<int32_t>(entity))).c_str());

        auto& tag = *context->registry.get<TagComponent>(entity);
        bool children = parentSystem.has_children(entity);

        ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                                   (children ? (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick) : ImGuiTreeNodeFlags_Leaf);

        if (entity != renameContext) {
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
        } else if (context->registry.valid(renameContext)) {
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
                ParentSystem{ context->registry }.assign_child(entity, *reinterpret_cast<entt::entity*>(payload->Data));
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
            parentSystem.each_child(entity, function);
            ImGui::TreePop();
        }
    };

    context->registry.each([&](auto entity) {
        if (parentSystem.get_parent(entity) == entt::null) {
            function(entity);
        }
    });

    if (removeEntity != entt::null) {
        //context->registry.destroy_parent(removeEntity);
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
            auto entity = context->registry.create();

            std::string name{ "Empty Entity" };
            size_t idx = 0;
            auto view = context->registry.view<const TagComponent>();
            for (auto [e, tag] : view.each()) {
                if (String::Contains(*tag, name)) {
                    idx++;
                }
            }
            if (idx > 0)
                name += " (" + std::to_string(idx) + ")";

            context->registry.emplace<TagComponent>(entity, name);
            context->registry.emplace<TransformComponent>(entity);

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
        ImGui::TextUnformatted(ICON_FA_SEARCH);
        ImGui::SameLine();
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, fileFilter.c_str(), sizeof(buffer));
        if (ImGui::InputTextWithHint("##filefilter", "Search File", buffer, sizeof(buffer))) {
            fileFilter = std::string{buffer};
            cachedFiles = Directory::GetFilesRecursive(std::filesystem::current_path(), fileFilter, formats);
        }

        ImGui::Separator();
        ImGui::BeginChild("FileBrowser", { 300.0f, 500.0f });

        for (const auto& file : cachedFiles) {
            std::string title{ File::ExtensionIcon(file) + " " + file.filename().string() };
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
            std::string title{ File::ExtensionIcon(currentFile) + " " + currentFile.string() };
            ImGui::TextUnformatted(title.c_str());
        }

        ImGui::EndPopup();
    }

    /*auto function = [&]() {
        std::string pattern{" "};
        for (const auto& format : formats) pattern += format + " ";
        auto filepath = pfd::open_file("Choose 3D file", value.empty() ? std::filesystem::current_path() : value, { file, pattern }, pfd::opt::none).result();
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
            cachedFiles = Directory::GetFilesRecursive(std::filesystem::current_path(), fileFilter, formats);
            ImGui::OpenPopup("FileExplorer");
        }
    } else {
        std::filesystem::path file{ value };
        std::string title{ File::ExtensionIcon(file) + " " + file.filename().string() };
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

    if (ImGui::Button(ICON_FA_SEARCH)) {
        fileFilter = "";
        currentFile = "";
        cachedFiles = Directory::GetFilesRecursive(std::filesystem::current_path(), fileFilter, formats);
        ImGui::OpenPopup("FileExplorer");
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

bool SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float minValue, float maxValue, float resetValue, float speedValue, float columnWidth) {
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
    modify += ImGui::DragFloat("##X", &values.x, speedValue, minValue, maxValue, "%.2f");
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
    modify += ImGui::DragFloat("##Y", &values.y, speedValue, minValue, maxValue, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        modify += 1;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    modify += ImGui::DragFloat("##Z", &values.z, speedValue, minValue, maxValue, "%.2f");

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modify;
}

bool SceneHierarchyPanel::drawVec2Control(const std::string& label, glm::vec2& values, float minValue, float maxValue, float resetValue, float speedValue, float columnWidth) {
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
    modify += ImGui::DragFloat("##X", &values.x, speedValue, minValue, maxValue, "%.2f");
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
    modify += ImGui::DragFloat("##Y", &values.y, speedValue, minValue, maxValue, "%.2f");

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

template<typename Enum>
bool SceneHierarchyPanel::drawEnumControl(const std::string& label, Enum& value, float columnWidth) {
    return drawValueControl(label, [&value]() {
        bool modify = false;
        constexpr auto entries = me::enum_entries<Enum>();
        if (ImGui::BeginCombo("", entries[me::enum_index(value).value_or(0)].second.data())) {
            for (const auto& [type, name] : entries) {
                bool isSelected = value == type;
                if (ImGui::Selectable(name.data(), isSelected)) {
                    value = type;
                    modify = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        return modify;
    }, columnWidth);
}

template<typename Component>
void SceneHierarchyPanel::drawComponent(const std::string& label, entt::entity entity, std::function<void(Component&)>&& function) {
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
                                     ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth |
                                     ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
    if (auto component = context->registry.try_get<Component>(entity)) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 4.0f });
        float lineHeight = ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y;
        ImGui::Separator();

        bool opened = ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), flags, "%s", label.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(lineHeight);

        if (ImGui::Button(ICON_FA_ELLIPSIS_V)) {
            ImGui::OpenPopup("ComponentSettings");
        }

        ComponentMode mode { None };
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Move Up"))
                mode = MoveUp;
            if (ImGui::MenuItem("Move Down"))
                mode = MoveDown;
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Component"))
                mode = Reset;
            if (ImGui::MenuItem("Remove Component"))
                mode = Remove;
            if (ImGui::MenuItem("Copy Component"))
                mode = Copy;
            if (ImGui::MenuItem("Paste Component As New"))
                mode = Paste;
            ImGui::EndPopup();
        }

        if (opened) {
            function(*component);
            ImGui::TreePop();
        }

        switch (mode) {
            case MoveUp:
                break;
            case MoveDown:
                break;
            case Reset:
                context->registry.replace<Component>(entity);
                break;
            case Remove:
                context->registry.erase<Component>(entity);
                break;
            case Copy:
                break;
            case Paste:
                break;
        }
    }
}

void SceneHierarchyPanel::drawComponents(entt::entity entity) {
    auto& tag = *context->registry.get<TagComponent>(entity);
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
        drawComponentMenuItem<TransformComponent>(ICON_FA_YELP + " Transform"s, entity);
        drawComponentMenuItem<CameraComponent>(ICON_FA_CAMERA + " Camera"s, entity);
        drawComponentMenuItem<MeshComponent>(ICON_FA_CODEPEN + " Mesh"s, entity);
        drawComponentMenuItem<RigidbodyComponent>(ICON_FA_CUBE + " Rigidbody"s, entity);
        drawComponentMenuItem<PhysicsMaterialComponent>(ICON_FA_TENCENT_WEIBO + " Physics Material"s, entity);
        drawComponentMenuItem<BoxColliderComponent>(ICON_FA_SQUARE_O + " Box Collider"s, entity);
        drawComponentMenuItem<SphereColliderComponent>(ICON_FA_CIRCLE_O + " Sphere Collider"s, entity);
        drawComponentMenuItem<CapsuleColliderComponent>(ICON_FA_TOGGLE_OFF + " Capsule Collider"s, entity);
        drawComponentMenuItem<MaterialComponent>(ICON_FA_DELICIOUS + " Material"s, entity);
        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<TransformComponent>(ICON_FA_YELP + "  Transform"s, entity, [&](TransformComponent& component)
    {
        uint8_t notify = 0;
        notify += drawVec3Control("Position", component.position);
        glm::vec3 rotation{ glm::degrees(glm::eulerAngles(component.rotation)) };
        notify += drawVec3Control("Rotation", rotation, -180.0f, 180.0f);
        component.rotation = glm::radians(rotation);
        notify += drawVec3Control("Scale", component.scale, 0.01f, FLT_MAX, 1.0f, 0.01f);

        if (notify)
            context->registry.patch<TransformComponent>(entity);
    });

    drawComponent<CameraComponent>(ICON_FA_CAMERA + "  Camera"s, entity, [&](CameraComponent& component)
    {
        auto& camera = component.camera;

        drawValueControl("Is Primary", [&component] { return ImGui::Checkbox("", &component.primary); });

        auto currentProjection = camera.getProjectionType();
        if (drawEnumControl<SceneCamera::ProjectionType>("Type", currentProjection)) {
            camera.setProjectionType(currentProjection);
        }

        switch (camera.getProjectionType()) {
            case SceneCamera::ProjectionType::Perspective: {
                drawValueControl("Vertical FOV", [&] {
                    float perspectiveVerticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("", &perspectiveVerticalFov)) {
                        camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));
                        return true;
                    }
                    return false;
                });
                drawValueControl("Near Clip", [&] {
                    float perspectiveNear = camera.getPerspectiveNearClip();
                    if (ImGui::DragFloat("", &perspectiveNear)) {
                        camera.setPerspectiveNearClip(perspectiveNear);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Far Clip", [&] {
                    float perspectiveFar = camera.getPerspectiveFarClip();
                    if (ImGui::DragFloat("", &perspectiveFar)) {
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
                    if (ImGui::DragFloat("", &orthoSize)) {
                        camera.setOrthographicSize(orthoSize);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Near Clip", [&] {
                    float orthoNear = camera.getOrthographicNearClip();
                    if (ImGui::DragFloat("", &orthoNear)) {
                        camera.setOrthographicNearClip(orthoNear);
                        return true;
                    }
                    return false;
                });
                drawValueControl("Far Clip", [&] {
                    float orthoFar = camera.getOrthographicFarClip();
                    if (ImGui::DragFloat("", &orthoFar)) {
                        camera.setOrthographicFarClip(orthoFar);
                        return true;
                    }
                    return false;
                });

                drawValueControl("Fixed Aspect", [&component] { return ImGui::Checkbox("", &component.fixedAspectRatio); });
                break;
            }
        }
    });

    drawComponent<MeshComponent>(ICON_FA_CODEPEN + "  Mesh"s, entity, [&](MeshComponent& component)
    {
        uint8_t notify = 0;
        notify += drawFileBrowser("File Path", component.path, { ".fbx", ".obj", ".dae", ".gltf", ".3ds" });
        notify += drawVec3Control("Scale", component.scale, 0.01f, FLT_MAX, 1.0f, 0.01f);
        notify += drawVec3Control("Center", component.center);
        notify += drawVec2Control("UV Scale", component.uvscale, 0.01f, FLT_MAX, 1.0f, 0.01f);
        if (notify)
            context->registry.patch<MeshComponent>(entity);
    });

    drawComponent<RigidbodyComponent>(ICON_FA_CUBE + "  Rigidbody"s, entity, [&](RigidbodyComponent& component)
    {
        drawEnumControl<RigidbodyComponent::BodyType>("Type", component.type);
        if (component.type == RigidbodyComponent::BodyType::Dynamic) {
            drawValueControl("Mass", [&component] { return ImGui::DragFloat("", &component.mass, 0.1f, 0.0f, FLT_MAX, "%.2f"); });
            drawValueControl("Linear Drag", [&component] { return ImGui::DragFloat("", &component.linearDrag, 0.1f, 0.0f, FLT_MAX, "%.2f"); });
            drawValueControl("Angular Drag", [&component] { return ImGui::DragFloat("", &component.angularDrag, 0.1f, 0.0f, FLT_MAX, "%.2f"); });
            drawValueControl("Disable Gravity", [&component] { return ImGui::Checkbox("", &component.disableGravity); });
            drawValueControl("Is Kinematic", [&component] { return ImGui::Checkbox("", &component.kinematic); });
        }
    });

    drawComponent<PhysicsMaterialComponent>(ICON_FA_TENCENT_WEIBO + "  Physics Material"s, entity, [&](PhysicsMaterialComponent& component)
    {
        drawValueControl("Dynamic Friction", [&component] { return ImGui::DragFloat("", &component.dynamicFriction, 0.01f, 0.0f, FLT_MAX, "%.2f"); });
        drawValueControl("Static Friction", [&component] { return ImGui::DragFloat("", &component.staticFriction, 0.01f, 0.0f, FLT_MAX, "%.2f"); });
        drawValueControl("Restitution", [&component] { return ImGui::DragFloat("", &component.restitution, 0.01f, 0.0f, 1.0f, "%.2f"); });
        drawEnumControl<PhysicsMaterialComponent::CombineMode>("Friction Combine", component.frictionCombine);
        drawEnumControl<PhysicsMaterialComponent::CombineMode>("Restitution Combine", component.restitutionCombine);
    });

    drawComponent<BoxColliderComponent>(ICON_FA_SQUARE_O + "  Box Collider"s, entity, [&](BoxColliderComponent& component)
    {
        drawVec3Control("Extent", component.extent, 0.01f, FLT_MAX, 1.0f, 0.01f);
        drawValueControl("Is Trigger", [&component]{ return ImGui::Checkbox("", &component.trigger); });
    });

    drawComponent<SphereColliderComponent>(ICON_FA_CIRCLE_O + "  Sphere Collider"s, entity, [&](SphereColliderComponent& component)
    {
        drawValueControl("Radius", [&component] { return ImGui::DragFloat("", &component.radius, 0.01f, 0.01f, FLT_MAX, "%.2f"); });
        drawValueControl("Is Trigger", [&component]{ return ImGui::Checkbox("", &component.trigger); });
    });

    drawComponent<CapsuleColliderComponent>(ICON_FA_TOGGLE_OFF + "  Capsule Collider"s, entity, [&](CapsuleColliderComponent& component)
    {
        drawValueControl("Radius", [&component] { return ImGui::DragFloat("", &component.radius, 0.01f, 0.01f, FLT_MAX, "%.2f"); });
        drawValueControl("Height", [&component] { return ImGui::DragFloat("", &component.height, 0.01f, 0.01f, FLT_MAX, "%.2f"); });
        drawValueControl("Is Trigger", [&component]{ return ImGui::Checkbox("", &component.trigger); });
    });

    drawComponent<MaterialComponent>(ICON_FA_DELICIOUS + "  Material"s, entity, [&](MaterialComponent& component)
    {

    });

    /*drawComponent<BoundsComponent>(fs::ICON_FA_BOUNDS + "  Bounds"s, entity, [&](BoundsComponent& component)
    {
        drawVec3Control("Min", component.min);
        drawVec3Control("Max", component.max);
    });*/
}

template<typename Component>
void SceneHierarchyPanel::drawComponentMenuItem(const std::string& label, entt::entity entity) {
    if (!context->registry.all_of<Component>(entity)) {
        if (ImGui::MenuItem(label.c_str())) {
            context->registry.emplace<Component>(entity);
            ImGui::CloseCurrentPopup();
        }
    }
}

/*if (ImGui::TreeNode("Layout")) {
    constexpr auto components = me::enum_entries<vkx::Component>();

    if (ImGui::BeginCombo("##Layout", "")) {
        for (const auto& [type, name] : components) {
            if (ImGui::Selectable(name.data(), false)) {
                component.layout.push_back(me::enum_index(type));
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear All"))
        component.layout.clear();

    for (int i = 0; i < component.layout.size(); i++) {
        auto id = component.layout[i];

        ImGui::Selectable(me::enum_name(me::enum_value<vkx::Component>(id)).data());

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