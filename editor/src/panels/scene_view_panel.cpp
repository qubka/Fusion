#include "scene_view_panel.h"
#include "editor.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/graphics/graphics.h"
#include "fusion/graphics/textures/texture2d.h"
#include "fusion/bitmaps/bitmap.h"
#include "fusion/devices/device_manager.h"
#include "fusion/input/input.h"
#include "fusion/debug/grid_subrender.h"
#include "fusion/debug/debug_renderer.h"

using namespace fe;

SceneViewPanel::SceneViewPanel(Editor& editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Scene###scene", "Scene", editor} {
    showComponentGizmosMap[type_id<LightComponent>] = true;
    showComponentGizmosMap[type_id<CameraComponent>] = true;
    //showComponentGizmosMap[type_id<SoundComponent>] = true;
}

SceneViewPanel::~SceneViewPanel() {

}

void SceneViewPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

    auto scene = SceneManager::Get()->getScene();
    if (!ImGui::Begin(title.c_str(), &active, flags) || !scene) {
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }

    {
        drawToolBar();
    }

    ImVec2 viewportOffset{ ImGui::GetCursorPos() };
    ImVec2 viewportSize{ ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() - viewportOffset * 0.5f };
    ImVec2 viewportPos{ ImGui::GetWindowPos() + viewportOffset };

    viewportSize.x -= static_cast<int>(viewportSize.x) % 2 != 0 ? 1.0f : 0.0f;
    viewportSize.y -= static_cast<int>(viewportSize.y) % 2 != 0 ? 1.0f : 0.0f;

    auto camera = editor.getCamera();
    if (!camera) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(viewportPos, viewportPos + viewportSize, IM_COL32(0, 0, 0, 255));
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetOrthographic(camera->isOrthographic());

    float aspect = viewportSize.x / viewportSize.y;
    camera->setAspectRatio(aspect);

    auto renderStage = Graphics::Get()->getRenderStage(0);
    renderStage->setOverrideCamera(camera);
    if (!renderStage->setViewport({glm::vec2{1.0f, 1.0f}, glm::uvec2{viewportSize.x, viewportSize.y}, glm::ivec2{0, 0}})) {
        ImGuiUtils::Image((Texture2d*)renderStage->getDescriptor("scene_image"), viewportSize, true);
    }

    ImVec2& minBound = viewportPos;
    ImVec2  maxBound{ minBound + ImGui::GetWindowSize() };

    bool viewportHovered = ImGui::IsMouseHoveringRect(minBound, maxBound); // || Input::Get().getMouseMode() == MouseMode::Captured;
    bool viewportFocused = ImGui::IsWindowFocused();

    editor.setSceneActive(viewportFocused && viewportHovered && !ImGuizmo::IsUsing());
    editor.setSceneViewActive(viewportHovered);
    editor.setSceneViewSize(viewportSize);

    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

    ImGui::GetWindowDrawList()->PushClipRect(viewportPos, { viewportSize.x + viewportPos.x, viewportSize.y + viewportPos.y - 2.0f });

    auto& registry = scene->getRegistry();

    drawGizmo(registry);

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            editor.fileOpenCallback(static_cast<const char*>(payload->Data));
        }
        ImGui::EndDragDropTarget();
    }

    if (!drawComponent(registry, viewportSize, viewportOffset)) {
        if (viewportFocused && viewportHovered && !ImGuizmo::IsUsing() && Input::Get()->getMouseButtonDown(MouseButton::ButtonLeft)) {
            glm::vec2 position{ Input::Get()->getMousePosition() - minBound };
            Ray ray = camera->screenPointToRay(position, viewportSize, true);
            editor.selectObject(ray, position);
        }
    }

    drawDebug(registry);

    ImGui::PopStyleVar();
    ImGui::End();
}

void SceneViewPanel::drawToolBar() {
    ImGui::Indent();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
    bool selected = false;

    {
        selected = editor.getSettings().gizmosOperation == UINT32_MAX;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_CURSOR_DEFAULT, selected))
            editor.getSettings().gizmosOperation = UINT32_MAX;
        ImGuiUtils::Tooltip("Select mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor.getSettings().gizmosOperation == ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ARROW_ALL, selected))
            editor.getSettings().gizmosOperation = ImGuizmo::TRANSLATE;
        ImGuiUtils::Tooltip("Translation mode");
    }

    {
        selected = editor.getSettings().gizmosOperation == ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ROTATE_3D, selected))
            editor.getSettings().gizmosOperation = ImGuizmo::ROTATE;
        ImGuiUtils::Tooltip("Rotatation mode");
    }

    {
        selected = editor.getSettings().gizmosOperation == ImGuizmo::SCALE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ARROW_EXPAND_ALL, selected))
            editor.getSettings().gizmosOperation = ImGuizmo::SCALE;
        ImGuiUtils::Tooltip("Scaling mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor.getSettings().gizmosOperation == ImGuizmo::UNIVERSAL;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_CROP_ROTATE, selected))
            editor.getSettings().gizmosOperation = ImGuizmo::UNIVERSAL;
        ImGuiUtils::Tooltip("Universal mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor.getSettings().gizmosOperation == ImGuizmo::BOUNDS;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_BORDER_NONE, selected))
            editor.getSettings().gizmosOperation = ImGuizmo::BOUNDS;
        ImGuiUtils::Tooltip("Bounds mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor.getSettings().snapGizmos;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_MAGNET, selected))
            editor.getSettings().snapGizmos = selected;
        ImGuiUtils::Tooltip("Snap enable");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor.getSettings().showGrid;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(selected ? ICON_MDI_GRID : ICON_MDI_GRID_OFF, selected)) {
            editor.getSettings().showGrid = selected;
            Graphics::Get()->getRenderer()->getSubrender<GridSubrender>()->setEnabled(selected);
        }
        ImGuiUtils::Tooltip("Toggle visibility of the grid");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        // Editor Camera Modes
        auto& camera = *editor.getCamera();
        bool ortho = camera.isOrthographic();

        selected = !ortho;
        if (ImGuiUtils::ToggleButton(ICON_MDI_AXIS_ARROW " 3D", selected)) {
            if (ortho) {
                camera.setOrthographic(false);
            }
        }

        ImGui::SameLine();

        selected = ortho;
        if (ImGuiUtils::ToggleButton(ICON_MDI_ANGLE_RIGHT "2D", selected)) {
            if (!ortho) {
                camera.setOrthographic(true);
                camera.lookAt(glm::vec3{0, 10, 0}, vec3::zero, vec3::up);
            }
        }
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    // TODO: Edit if needed
    float xAvail = ImGui::GetContentRegionAvail().x;
    ImGui::SameLine(xAvail > 500.0f ? xAvail - 130.0f : 0.0f);

    if (ImGui::Button(ICON_MDI_CAMERA_WIRELESS " " ICON_MDI_CHEVRON_DOWN))
        ImGui::OpenPopup("CameraPopup");
    if (ImGui::BeginPopup("CameraPopup", ImGuiWindowFlags_AlwaysAutoResize)) {
        {
            // Editor Camera Settings
            auto& camera = *editor.getCamera();
            bool ortho = camera.isOrthographic();

            ImGui::Dummy(ImVec2{200.0f, 0.0f});  // fix resize
            ImGui::TextUnformatted(" Scene camera");
            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);

            if (ortho) {
                const float fov = camera.getFov();
                ImGuiUtils::Property("Fov", fov);
            } else {
                float fov = camera.getFov();
                if (ImGuiUtils::Property("Fov", fov, 1.0f, 120.0f, 1.0f)) {
                    camera.setFov(fov);
                }
            }

            float nearClip = camera.getNearClip();
            if (ImGuiUtils::Property("Near Clip", nearClip, 0.0f, 10.0f)) {
                camera.setNearClip(nearClip);
            }

            float farClip = camera.getFarClip();
            if (ImGuiUtils::Property("Far Clip", farClip, 10.0f, 10000.0f)) {
                camera.setFarClip(farClip);
            }

            ImGui::Columns(1);
            ImGui::PopStyleVar();

            ImGui::EndPopup();
        }
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    if (ImGui::Button(ICON_MDI_SHAPE_OUTLINE " Gizmos " ICON_MDI_CHEVRON_DOWN))
        ImGui::OpenPopup("GizmosPopup");
    if (ImGui::BeginPopup("GizmosPopup", ImGuiWindowFlags_AlwaysAutoResize)) {
        {
            ImGui::Checkbox("Selected Gizmos", &editor.getSettings().showGizmos);

            ImGui::Separator();
            ImGui::Checkbox("Camera", reinterpret_cast<bool*>(&showComponentGizmosMap[type_id<CameraComponent>]));
            ImGui::Checkbox("Light", reinterpret_cast<bool*>(&showComponentGizmosMap[type_id<LightComponent>]));
            //ImGui::Checkbox("Audio", (bool*)showComponentGizmosMap[type_id<SoundComponent>]);

            ImGui::Separator();

            auto& flags = editor.getSettings().debugDrawFlags;

            ImGui::CheckboxFlags("Mesh AABB", &flags, EditorDebugFlags::MeshBoundingBoxes);
            ImGui::CheckboxFlags("Sprite Box", &flags, EditorDebugFlags::SpriteBoxes);
            ImGui::CheckboxFlags("Camera Frustums", &flags, EditorDebugFlags::CameraFrustum);

            ImGui::Separator();

            ImGui::EndPopup();
        }
    }

    ImGui::PopStyleColor();
    ImGui::Unindent();
}

void SceneViewPanel::drawDebug(entt::registry& registry) {
    if (!editor.getSettings().showGizmos)
        return;

    glm::vec4 selectedColour{0.9f};

    if (editor.getSettings().debugDrawFlags & EditorDebugFlags::MeshBoundingBoxes) {
        auto view = registry.view<TransformComponent, MeshComponent>();

        for (const auto& [entity, transform, mesh] : view.each()) {
            auto filter = mesh.get();
            if (!filter)
                continue;

            const auto& worldTransform = transform.getWorldMatrix();

            auto bbCopy = filter->getBoundingBox().transformed(worldTransform);

            DebugRenderer::DebugDraw(bbCopy, selectedColour, true);
        }
    }

    if (editor.getSettings().debugDrawFlags & EditorDebugFlags::CameraFrustum) {
        auto view = registry.view<TransformComponent, CameraComponent>();

        for (const auto& [entity, transform, camera] : view.each()) {
            DebugRenderer::DebugDraw(camera.getFrustum(), selectedColour);
        }
    }

    auto selected = editor.getSelected();
    
    if (registry.valid(selected)) {
        auto [transform, mesh] = registry.try_get<TransformComponent, MeshComponent>(selected);
        if (transform && mesh) {
            if (auto filter = mesh->get()) {
                const auto& worldTransform = transform->getWorldMatrix();

                auto bbCopy = filter->getBoundingBox().transformed(worldTransform);

                DebugRenderer::DebugDraw(bbCopy, selectedColour, true);
            }
        }

        if (auto camera = registry.try_get<CameraComponent>(selected)) {
            DebugRenderer::DebugDraw(camera->getFrustum(), selectedColour);
        }

        auto light = registry.try_get<LightComponent>(selected);
        if (light && transform) {
            DebugRenderer::DebugDraw(*light, transform->getWorldPosition(), transform->getWorldOrientation(), transform->getWorldForwardDirection(), light->color);
        }
    }
}

void SceneViewPanel::drawGizmo(entt::registry& registry) {
    if (!editor.getSettings().showGizmos || editor.getSettings().gizmosOperation == UINT32_MAX)
        return;

    auto selected = editor.getSelected();
    if (!registry.valid(selected))
        return;

    if (auto transform = registry.try_get<TransformComponent>(selected)) {
        auto camera = editor.getCamera();
        const glm::mat4& view = camera->getViewMatrix();
        const glm::mat4& proj = camera->getProjectionMatrix();
        glm::mat4 model = transform->getWorldMatrix();
        glm::mat4 delta{1};

        auto gizmosType = static_cast<ImGuizmo::OPERATION>(editor.getSettings().gizmosOperation);

        // Snapping
        float snapValue = editor.getSettings().snapAmount; // Snap to 0.5m for translation/scale
        if (gizmosType == ImGuizmo::ROTATE)
            snapValue = 45.0f;
        glm::vec3 snapValues{ snapValue };

        // Bounding
        auto bounds = gizmosType == ImGuizmo::BOUNDS;
        glm::vec3 boundsSnap{ editor.getSettings().snapBound };  // Snap to 0.1m for bound change
        static glm::mat2x3 boundsValues = { glm::vec3{-1.0f}, glm::vec3{1.0f} };

        ImGuizmo::Manipulate(glm::value_ptr(view),
                             glm::value_ptr(proj),
                             gizmosType,
                             ImGuizmo::LOCAL,
                             glm::value_ptr(model),
                             glm::value_ptr(delta),
                             editor.getSettings().snapGizmos ? glm::value_ptr(snapValues) : nullptr,
                             bounds ? glm::value_ptr(boundsValues) : nullptr,
                             bounds ? glm::value_ptr(boundsSnap) : nullptr);

        if (ImGuizmo::IsUsing()) {
            model = glm::inverse(transform->getParentMatrix()) * model;

            glm::vec3 position, scale;
            glm::quat orientation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(model, scale, orientation, position, skew, perspective);

            switch (gizmosType) {
                case ImGuizmo::TRANSLATE:
                    transform->setLocalPosition(position);
                    break;
                case ImGuizmo::ROTATE:
                    transform->setLocalOrientation(orientation);
                    break;
                case ImGuizmo::SCALE:
                    transform->setLocalScale(scale);
                    break;
                default:
                    transform->setLocalPosition(position);
                    transform->setLocalOrientation(orientation);
                    transform->setLocalScale(scale);
                    break;
            }
        }
    }
}


#define DRAW_COMPONENT(ComponentType) hovered |= drawComponentGizmos<ComponentType>(registry, camera, coord, offset, #ComponentType);

bool SceneViewPanel::drawComponent(entt::registry& registry, const glm::vec2& coord, const glm::vec2& offset) {
    auto& camera = *editor.getCamera();

    bool hovered = false;
    DRAW_COMPONENT(LightComponent);
    DRAW_COMPONENT(CameraComponent);
    //DRAW_COMPONENT(SoundComponent);
    return hovered;
}
