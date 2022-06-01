#include "scene_hierarchy_panel.hpp"
#include "content_browser_panel.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/utils/files.hpp"

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
    ImGui::Begin((fs::ICON_FA_LIST + "  Hierarchy"s).c_str());

    ImGui::TextUnformatted(fs::ICON_FA_SEARCH);
    ImGui::SameLine();
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, filter.c_str(), sizeof(buffer));
    if (ImGui::InputTextWithHint("##entityfilter", "Search Entities", buffer, sizeof(buffer))) {
        filter = std::string{buffer};
    }

    ImGui::Separator();

    context->registry.each([&](auto entity) {
       drawEntity(entity);
   });

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        selectionContext = entt::null;
        renameContext = entt::null;
    }

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow("HierarchyOptions", 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            auto entity = context->registry.create();
            context->registry.emplace<TagComponent>(entity, "Empty Entity");
            context->registry.emplace<TransformComponent>(entity);
            context->registry.emplace<RelationshipComponent>(entity);
        }
        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin((fs::ICON_FA_INFO + "  Inspector"s).c_str());
    if (selectionContext != entt::null) {
        drawComponents(selectionContext);
    }

    ImGui::End();
}

void SceneHierarchyPanel::drawEntity(entt::entity entity) {
    auto& tag = *context->registry.get<TagComponent>(entity);

    ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    if (entity != renameContext) {
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
    } else if (renameContext != entt::null) {
        flags |= ImGuiTreeNodeFlags_FramePadding;

        float lineHeight = GImGui->Font->FontSize / 2.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { lineHeight, lineHeight });
    }
    bool opened = ImGui::TreeNodeEx((void*)entity, flags, "");
    if (ImGui::IsItemClicked()/* || ImGui::IsItemFocused()*/) {
        selectionContext = entity;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        renameContext = entity;
    }

    ImGui::SameLine();

    if (renameContext == entity) {
        if (!ImGui::IsWindowFocused()) {
            renameContext = entt::null;
        } else {
            if (ImGui::IsItemFocused()) {
                ImGui::SetKeyboardFocusHere();
            }
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("##rename", buffer, sizeof(buffer))) {
                tag = std::string{buffer};
            }
            //ImGui::SameLine();
            if (/*ImGui::Button(fs::ICON_FA_OK) || */ImGui::IsKeyDown(ImGuiKey_Enter)) {
                renameContext = entt::null;
            }
        }
    } else {
        //ImGui::TextUnformatted(tag.c_str());
        auto id = std::to_string(static_cast<int>(entity));
        ImGui::Button(("entity "s + id).c_str());

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("SCENE_HIERARCHY_ITEM", id.c_str(), id.length() + 1);
            ImGui::EndDragDropSource();
        }

        /*if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {

            }
            ImGui::EndDragDropTarget();
        }*/
    }

    if (ImGui::BeginPopupContextItem("EntityOptions")) {
        if (ImGui::MenuItem("Copy")) {
        }
        if (ImGui::MenuItem("Paste")) {
        }
        ImGui::Spacing();
        if (ImGui::MenuItem("Rename")) {
            selectionContext = entity;
            renameContext = entity;
        }
        if (ImGui::MenuItem("Duplicate")) {

        }
        if (ImGui::MenuItem("Delete")) {
            context->registry.destroy(entity);
            if (selectionContext == entity)
                selectionContext = entt::null;
            if (renameContext == entity)
                renameContext = entt::null;
        }
        ImGui::EndPopup();
    }

    if (opened) {
        //if (ImGui::TreeNodeEx((void*)9817239, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth, "%s", tag.c_str()))
        //    ImGui::TreePop();
        ImGui::TreePop();
    }

    if (flags & ImGuiTreeNodeFlags_FramePadding)
        ImGui::PopStyleVar();
}

void SceneHierarchyPanel::drawFileBrowser(const std::string& label, std::string& value, const std::string& file, const std::vector<std::string>& formats, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.0f, 4.0f });

    auto function = [&]() {
        std::string pattern{" "};
        for (const auto& format : formats) pattern += format + " ";
        auto filepath = pfd::open_file("Choose 3D file", value.empty() ? getAssetPath() : value, { file, pattern }, pfd::opt::none).result();
        if (!filepath.empty()) {
            // Validate that file inside working directory
            if (filepath[0].find(std::filesystem::current_path()) != std::string::npos) {
                value = std::filesystem::relative(filepath[0]);
            } else {
                pfd::message("File Location", "The selected file should be inside the project directory.", pfd::choice::ok, pfd::icon::error);
            }
        }
    };

    ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x - lineHeight, lineHeight };

    if (value.empty()) {
        if (ImGui::Button("...", buttonSize)) {
            function();
        }
    } else {
        std::filesystem::path path{value};
        auto title = fs::extension_icon(path) + " " + path.filename().string();
        if (ImGui::Button(title.c_str(), buttonSize)) {
            contentBrowserPanel.selectFile(path);
        }
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        value = "";
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            auto filepath = std::filesystem::path{static_cast<const char*>(payload->Data)};
            auto extension = "*" + filepath.extension().string();
            // Validate that file format is suitable
            if (std::find(formats.begin(), formats.end(), extension) != formats.end()) {
                value = filepath;
            } else {
                pfd::message("File Format", "The selected file format should be " + file + ".", pfd::choice::ok, pfd::icon::error);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();

    if (ImGui::Button(fs::ICON_FA_SEARCH)) {
        function();
    }

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

void SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
    //ImGuiIO& io = ImGui::GetIO();
    //auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        //return true;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        //return true;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
    //::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        //return true;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    //return false;
}

void SceneHierarchyPanel::drawVec2Control(const std::string& label, glm::vec2& values, float resetValue, float columnWidth) {
    //ImGuiIO& io = ImGui::GetIO();
    //auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::TextUnformatted(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        //return true;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    //ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        //return true;
    }
    //ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    //return false;
}

template<typename T>
void SceneHierarchyPanel::drawComponent(const std::string& name, entt::entity entity, std::function<void(T& comp)>&& function, bool removable) {
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
    if (auto component = context->registry.try_get<T>(entity)) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 4.0f });
        ImGui::Separator();

        bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", name.c_str());
        ImGui::PopStyleVar();

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight * 0.5f);

        if (ImGui::Button(fs::ICON_FA_REMOVE)) {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (removable && ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (opened) {
            function(*component);
            ImGui::TreePop();
        }

        if (removeComponent)
            context->registry.remove<T>(entity);
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
        if (!context->registry.all_of<CameraComponent>(entity)) {
            if (ImGui::MenuItem("Camera")) {
                context->registry.emplace<CameraComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        if (!context->registry.all_of<ModelComponent>(entity)) {
            if (ImGui::MenuItem("Model")) {
                context->registry.emplace<ModelComponent>(entity);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<TransformComponent>(fs::ICON_FA_AXIS + "  Transform"s, entity, [](TransformComponent& component)
    {
        drawVec3Control("Translation", component.translation);
        glm::vec3 rotation = glm::degrees(component.rotation);
        drawVec3Control("Rotation", rotation);
        component.rotation = glm::radians(rotation);
        drawVec3Control("Scale", component.scale, 1.0f);
    }, false);

    drawComponent<CameraComponent>(fs::ICON_FA_CAMERA + "  Camera"s, entity, [](CameraComponent& component)
    {
        auto& camera = component.camera;

        ImGui::Checkbox("Primary", &component.primary);

        constexpr auto projections = magic_enum::enum_entries<SceneCamera::ProjectionType>();
        auto currentProjection = camera.getProjectionType();

        if (ImGui::BeginCombo("Projection", projections[magic_enum::enum_integer(currentProjection)].second.data())) {
            for (const auto& [type, name] : projections) {
                bool isSelected = currentProjection == type;
                if (ImGui::Selectable(name.data(), isSelected)) {
                    currentProjection = type;
                    camera.setProjectionType(type);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        switch (camera.getProjectionType()) {
            case SceneCamera::ProjectionType::Perspective: {
                float perspectiveVerticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
                    camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

                float perspectiveNear = camera.getPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &perspectiveNear))
                    camera.setPerspectiveNearClip(perspectiveNear);

                float perspectiveFar = camera.getPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &perspectiveFar))
                    camera.setPerspectiveFarClip(perspectiveFar);
                break;
            }

            case SceneCamera::ProjectionType::Orthographic: {
                float orthoSize = camera.getOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                    camera.setOrthographicSize(orthoSize);

                float orthoNear = camera.getOrthographicNearClip();
                if (ImGui::DragFloat("Near", &orthoNear))
                    camera.setOrthographicNearClip(orthoNear);

                float orthoFar = camera.getOrthographicFarClip();
                if (ImGui::DragFloat("Far", &orthoFar))
                    camera.setOrthographicFarClip(orthoFar);

                ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
                break;
            }
        }
    });

    drawComponent<ModelComponent>(fs::ICON_FA_CUBES + "  Model"s, entity, [&](ModelComponent& component)
    {
        drawFileBrowser("Path", component.path, "3D Files (.fbx .obj .dae .gltf .3ds)", { "*.fbx", "*.obj", "*.dae", "*.gltf", "*.3ds" });

        drawVec3Control("Scale", component.scale);
        drawVec3Control("Center", component.center);
        drawVec2Control("UV Scale", component.uvscale);

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
}