#include "inspector_panel.hpp"
#include "editor.hpp"

#include "fusion/scene/components.hpp"

using namespace fe;

namespace ImGui {
    template<>
    void ComponentEditorWidget<TransformComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& transform = registry.get<TransformComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        glm::vec3 position{ transform.getLocalPosition() };
        if (ImGuiUtils::PropertyControl("Position", position)) {
            transform.setLocalPosition(position);
        }

        glm::vec3 rotation{ glm::degrees(glm::eulerAngles(transform.getLocalOrientation())) };
        if (ImGuiUtils::PropertyControl("Rotation", rotation, -180.0f, 180.0f)) {
            transform.setLocalOrientation(glm::radians(rotation));
        }

        glm::vec3 scale{ transform.getLocalScale() };
        if (ImGuiUtils::PropertyControl("Scale", scale, 0.01f, FLT_MAX, 1.0f, 0.01f)) {
            transform.setLocalScale(scale);
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<CameraComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& camera = registry.get<CameraComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        /*ImGuiUtils::Property("Is Primary", camera.primary);

        auto& sceneCamera = camera.camera;

        auto currentProjection = sceneCamera.getProjectionType();
        if (ImGuiUtils::PropertyDropdown("Type", currentProjection)) {
            sceneCamera.setProjectionType(currentProjection);
        }

        float fov = sceneCamera.getFOV();
        if (ImGuiUtils::Property(sceneCamera.isOrthographic() ? "Size" : "Fov", fov, 1.0f, 120.0f)) {
            sceneCamera.setFOV(fov);
        }

        float nearClip = sceneCamera.getNearClip();
        if (ImGuiUtils::Property("Near Clip", nearClip, 0.0f, 10.0f)) {
            sceneCamera.setNearClip(nearClip);
        }

        float farClip = sceneCamera.getFarClip();
        if (ImGuiUtils::Property("Far Clip", farClip, 10.0f, 10000.0f)) {
            sceneCamera.setFarClip(farClip);
        }*/

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }
}

#define REG_COMPONENT(ComponentType, ComponentName)            \
{                                                              \
    std::string name;                                          \
    editor->getComponentIcon(typeid(ComponentType), name);     \
    name += "\t";                                              \
    name += (ComponentName);                                   \
    enttEditor.registerComponent<ComponentType>(name.c_str()); \
}

InspectorPanel::InspectorPanel(Editor* editor) : EditorPanel(ICON_MDI_INFORMATION " Inspector###inspector", "Inspector", editor) {
    REG_COMPONENT(TransformComponent, "Transform");
    REG_COMPONENT(ModelComponent, "Model");
    REG_COMPONENT(CameraComponent, "Camera");
}

InspectorPanel::~InspectorPanel() {

}

void InspectorPanel::onImGui() {
    auto selected = editor->getSelected();

    if (ImGui::Begin(title.c_str(), &active)) {
        auto scene = SceneManager::Get()->getScene();
        if (!scene) {
            editor->setSelected(entt::null);
            ImGui::End();
            return;
        }

        auto& registry = scene->getRegistry();
        if (selected == entt::null || !registry.valid(selected)) {
            editor->setSelected(entt::null);
            ImGui::End();
            return;
        }

        // active checkbox
        auto activeComponent = registry.try_get<ActiveComponent>(selected);
        bool active = activeComponent ? activeComponent->active : true;
        if (ImGui::Checkbox("##ActiveCheckbox", &active)) {
            if (!activeComponent)
                registry.emplace<ActiveComponent>(selected, active);
            else
                activeComponent->active = active;
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(ICON_MDI_CUBE);
        ImGui::SameLine();

        auto nameComponent = registry.try_get<NameComponent>(selected);
        std::string name{ nameComponent ? *nameComponent : std::to_string(entt::to_integral(selected)) };

        if (debugMode) {
            ImGui::Text("ID: %d", static_cast<int>(selected));
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 2.0f);
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            if (ImGuiUtils::InputText("##InsEntityName", name))
                registry.get_or_emplace<NameComponent>(selected).name = name;
            ImGui::PopFont();
        }
        ImGui::SameLine();

        ImVec4 color{0.7f, 0.7f, 0.7f, 0.0f};
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));

        if (ImGui::Button(ICON_MDI_TUNE))
            ImGui::OpenPopup("SetDebugMode");

        ImGui::PopStyleColor(3);

        if (ImGui::BeginPopup("SetDebugMode", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
            if (ImGui::Selectable("Debug Mode", debugMode)) {
                debugMode = !debugMode;
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        if (debugMode) {
            auto idComponent = registry.try_get<IdComponent>(selected);

            ImGui::Text("UUID: %s", uuids::to_string(idComponent->uuid).c_str());

            if (auto hierarchyComponent = registry.try_get<HierarchyComponent>(selected)) {
                auto parent = hierarchyComponent->parent;
                if (parent != entt::null) {
                    ImGui::Text("Parent : ID: %d", static_cast<int>(parent));
                    if (auto nameComponent = registry.try_get<NameComponent>(parent)) {
                        ImGui::SameLine();
                        ImGui::Text(" (%s)", nameComponent->name.c_str());
                    }
                } else {
                    ImGui::TextUnformatted("Parent : null");
                }

                ImGui::Text("Children : %d", hierarchyComponent->children);

                ImGui::Indent(24.0f);

                auto child = hierarchyComponent->first;
                while (child != entt::null) {
                    ImGui::Text("ID: %d", static_cast<int>(child));
                    if (auto nameComponent = registry.try_get<NameComponent>(child)) {
                        ImGui::SameLine();
                        ImGui::Text(" (%s)", nameComponent->name.c_str());
                    }
                    child = registry.get<HierarchyComponent>(child).next;
                }

                ImGui::Unindent(24.0f);
            }

            ImGui::Separator();
        }

        ImGui::BeginChild("Components", ImVec2{0.0f, 0.0f}, false, ImGuiWindowFlags_None);
        enttEditor.Render(registry, selected);
        ImGui::EndChild();
    }
    ImGui::End();
}
