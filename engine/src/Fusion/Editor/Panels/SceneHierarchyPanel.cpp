#include "SceneHierarchyPanel.hpp"
#include "Fusion/Scene/Components.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <magic_enum.hpp>

using namespace Fusion;

void SceneHierarchyPanel::setContext(const std::shared_ptr<Scene>& scene) {
    context = scene;
    selectionContext = entt::null;
}

void SceneHierarchyPanel::onImGui() {
    ImGui::Begin("Scene Hierarchy");

    context->registry.each([&](auto entity) {
       drawEntity(entity);
   });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        selectionContext = entt::null;

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            auto entity = context->registry.create();
            context->registry.emplace<TagComponent>(entity, "Empty Entity");
            context->registry.emplace<TransformCompositionComponent>(entity);
        }
        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (selectionContext != entt::null) {
        drawComponents(selectionContext);
    }

    ImGui::End();
}

void SceneHierarchyPanel::drawEntity(entt::entity entity) {
    auto tag = *context->registry.get<TagComponent>(entity);

    ImGuiTreeNodeFlags flags = ((selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void*)static_cast<uintptr_t>(entity), flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked()) {
        selectionContext = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete Entity"))
            entityDeleted = true;
        ImGui::EndPopup();
    }

    if (opened) {
        //if (ImGui::TreeNodeEx((void*)9817239, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth, "%s", tag.c_str()))
        //    ImGui::TreePop();
        ImGui::TreePop();
    }

    if (entityDeleted) {
        context->registry.destroy(entity);
        if (selectionContext == entity)
            selectionContext = entt::null;
    }
}

void SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

template<typename T, typename F>
void SceneHierarchyPanel::drawComponent(const std::string& name, entt::entity entity, F func) {
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (auto* component {context->registry.try_get<T>(entity)}; component != nullptr) {
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();

        bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (opened) {
            func(*component);
            ImGui::TreePop();
        }

        if (removeComponent)
            context->registry.remove<T>(entity);
    }
}

void SceneHierarchyPanel::drawComponents(entt::entity entity)
{
    auto& tag = *context->registry.get<TagComponent>(entity);
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, tag.c_str(), sizeof(buffer));
    if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
        tag = std::string(buffer);
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponent");

    if (ImGui::BeginPopup("AddComponent")) {
        if (!context->registry.all_of<CameraComponent>(selectionContext)) {
            if (ImGui::MenuItem("Camera")) {
                context->registry.emplace<CameraComponent>(selectionContext);
                ImGui::CloseCurrentPopup();
            }
        }


        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<TransformCompositionComponent>("Transform", entity, [this](auto& component)
    {
        drawVec3Control("Translation", component.translation);
        glm::vec3 rotation = glm::degrees(component.rotation);
        drawVec3Control("Rotation", rotation);
        component.rotation = glm::radians(rotation);
        drawVec3Control("Scale", component.scale, 1.0f);
    });

    drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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

    /*DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
    {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
    });*/
}