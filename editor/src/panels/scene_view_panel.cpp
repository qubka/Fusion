#include "scene_view_panel.hpp"
#include "editor.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/input/input.hpp"
#include "fusion/grid/grid_subrender.hpp"

using namespace fe;

SceneViewPanel::SceneViewPanel(Editor* editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Scene###scene", "Scene", editor} {
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

    auto camera = editor->getCamera();
    if (!camera) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(viewportPos, viewportPos + viewportSize, IM_COL32(0, 0, 0, 255));
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }

    ImGuizmo::SetDrawlist();

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

    editor->setSceneActive(viewportFocused && viewportHovered && !ImGuizmo::IsUsing());
    editor->setSceneViewActive(viewportHovered);
    editor->setSceneViewSize(viewportSize);

    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

    ImGui::GetWindowDrawList()->PushClipRect(viewportPos, { viewportSize.x + viewportPos.x, viewportSize.y + viewportPos.y - 2.0f });

    editor->onImGuizmo();

    if (editor->isSceneActive() && Input::Get()->getMouseButton(MouseButton::ButtonLeft)) {
        Ray ray = camera->screenPointToRay(Input::Get()->getMousePosition() - minBound, viewportSize, true);
        editor->selectObject(ray);
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            editor->fileOpenCallback(static_cast<const char*>(payload->Data));
        }
        ImGui::EndDragDropTarget();
    }

    drawGizmos(scene->getRegistry(), *camera, viewportSize, viewportOffset);

    ImGui::PopStyleVar();
    ImGui::End();
}

void SceneViewPanel::drawToolBar() {
    ImGui::Indent();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
    bool selected = false;

    {
        selected = editor->getSettings().gizmosOperation == UINT32_MAX;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_CURSOR_DEFAULT, selected))
            editor->getSettings().gizmosOperation = UINT32_MAX;
        ImGuiUtils::Tooltip("Select mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ARROW_ALL, selected))
            editor->getSettings().gizmosOperation = ImGuizmo::TRANSLATE;
        ImGuiUtils::Tooltip("Translation mode");
    }

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ROTATE_3D, selected))
            editor->getSettings().gizmosOperation = ImGuizmo::ROTATE;
        ImGuiUtils::Tooltip("Rotatation mode");
    }

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::SCALE;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_ARROW_EXPAND_ALL, selected))
            editor->getSettings().gizmosOperation = ImGuizmo::SCALE;
        ImGuiUtils::Tooltip("Scaling mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::UNIVERSAL;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_CROP_ROTATE, selected))
            editor->getSettings().gizmosOperation = ImGuizmo::UNIVERSAL;
        ImGuiUtils::Tooltip("Universal mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::BOUNDS;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_BORDER_NONE, selected))
            editor->getSettings().gizmosOperation = ImGuizmo::BOUNDS;
        ImGuiUtils::Tooltip("Bounds mode");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().snapGizmos;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_MAGNET, selected))
            editor->getSettings().snapGizmos = selected;
        ImGuiUtils::Tooltip("Snap enable");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().showGrid;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(selected ? ICON_MDI_GRID : ICON_MDI_GRID_OFF, selected)) {
            editor->getSettings().showGrid = selected;
            Graphics::Get()->getRenderer()->getSubrender<GridSubrender>()->setEnabled(selected);
        }
        ImGuiUtils::Tooltip("Toggle visibility of the grid");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        // Editor Camera Modes
        auto& camera = *editor->getCamera();
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
            auto& camera = *editor->getCamera();
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
            ImGui::Checkbox("Selected Gizmos", &editor->getSettings().showGizmos);

            ImGui::Separator();
            ImGui::Checkbox("Camera", &showComponentGizmosMap[type_id<CameraComponent>]);
            ImGui::Checkbox("Light", &showComponentGizmosMap[type_id<LightComponent>]);
            //ImGui::Checkbox("Audio", &showComponentGizmosMap[type_id<SoundComponent>]);

            /*ImGui::Separator();

            uint32_t flags = m_Editor->GetSettings().m_DebugDrawFlags;

            bool showAABB = flags & EditorDebugFlags::MeshBoundingBoxes;
            if (ImGui::Checkbox("Mesh AABB", &showAABB)) {
                if (showAABB)
                    flags += EditorDebugFlags::MeshBoundingBoxes;
                else
                    flags -= EditorDebugFlags::MeshBoundingBoxes;
            }

            bool showSpriteBox = flags & EditorDebugFlags::SpriteBoxes;
            if (ImGui::Checkbox("Sprite Box", &showSpriteBox)) {
                if (showSpriteBox)
                    flags += EditorDebugFlags::SpriteBoxes;
                else
                    flags -= EditorDebugFlags::SpriteBoxes;
            }

            bool showCameraFrustums = flags & EditorDebugFlags::CameraFrustum;
            if (ImGui::Checkbox("Camera Frustums", &showCameraFrustums)) {
                if (showCameraFrustums)
                    flags += EditorDebugFlags::CameraFrustum;
                else
                    flags -= EditorDebugFlags::CameraFrustum;
            }

            m_Editor->GetSettings().m_DebugDrawFlags = flags;
            ImGui::Separator();

            auto physics2D = Application::Get().GetSystem<B2PhysicsEngine>();

            if (physics2D) {
                uint32_t flags = physics2D->GetDebugDrawFlags();

                bool show2DShapes = flags & b2Draw::e_shapeBit;
                if (ImGui::Checkbox("Shapes (2D)", &show2DShapes)) {
                    if (show2DShapes)
                        flags += b2Draw::e_shapeBit;
                    else
                        flags -= b2Draw::e_shapeBit;
                }

                bool showCOG = flags & b2Draw::e_centerOfMassBit;
                if (ImGui::Checkbox("Centre of Mass (2D)", &showCOG)) {
                    if (showCOG)
                        flags += b2Draw::e_centerOfMassBit;
                    else
                        flags -= b2Draw::e_centerOfMassBit;
                }

                bool showJoint = flags & b2Draw::e_jointBit;
                if (ImGui::Checkbox("Joint Connection (2D)", &showJoint)) {
                    if (showJoint)
                        flags += b2Draw::e_jointBit;
                    else
                        flags -= b2Draw::e_jointBit;
                }

                bool showAABB = flags & b2Draw::e_aabbBit;
                if (ImGui::Checkbox("AABB (2D)", &showAABB)) {
                    if (showAABB)
                        flags += b2Draw::e_aabbBit;
                    else
                        flags -= b2Draw::e_aabbBit;
                }

                bool showPairs = static_cast<bool>(flags & b2Draw::e_pairBit);
                if (ImGui::Checkbox("Broadphase Pairs  (2D)", &showPairs)) {
                    if (showPairs)
                        flags += b2Draw::e_pairBit;
                    else
                        flags -= b2Draw::e_pairBit;
                }

                physics2D->SetDebugDrawFlags(flags);
            }

            auto physics3D = Application::Get().GetSystem<LumosPhysicsEngine>();

            if (physics3D) {
                uint32_t flags = physics3D->GetDebugDrawFlags();

                bool showCollisionShapes = flags & PhysicsDebugFlags::COLLISIONVOLUMES;
                if (ImGui::Checkbox("Collision Volumes", &showCollisionShapes)) {
                    if (showCollisionShapes)
                        flags += PhysicsDebugFlags::COLLISIONVOLUMES;
                    else
                        flags -= PhysicsDebugFlags::COLLISIONVOLUMES;
                }

                bool showConstraints = static_cast<bool>(flags & PhysicsDebugFlags::CONSTRAINT);
                if (ImGui::Checkbox("Constraints", &showConstraints)) {
                    if (showConstraints)
                        flags += PhysicsDebugFlags::CONSTRAINT;
                    else
                        flags -= PhysicsDebugFlags::CONSTRAINT;
                }

                bool showManifolds = static_cast<bool>(flags & PhysicsDebugFlags::MANIFOLD);
                if (ImGui::Checkbox("Manifolds", &showManifolds)) {
                    if (showManifolds)
                        flags += PhysicsDebugFlags::MANIFOLD;
                    else
                        flags -= PhysicsDebugFlags::MANIFOLD;
                }

                bool showCollisionNormals = flags & PhysicsDebugFlags::COLLISIONNORMALS;
                if (ImGui::Checkbox("Collision Normals", &showCollisionNormals)) {
                    if (showCollisionNormals)
                        flags += PhysicsDebugFlags::COLLISIONNORMALS;
                    else
                        flags -= PhysicsDebugFlags::COLLISIONNORMALS;
                }

                bool showAABB = flags & PhysicsDebugFlags::AABB;
                if (ImGui::Checkbox("AABB", &showAABB)) {
                    if (showAABB)
                        flags += PhysicsDebugFlags::AABB;
                    else
                        flags -= PhysicsDebugFlags::AABB;
                }

                bool showLinearVelocity = flags & PhysicsDebugFlags::LINEARVELOCITY;
                if (ImGui::Checkbox("Linear Velocity", &showLinearVelocity)) {
                    if (showLinearVelocity)
                        flags += PhysicsDebugFlags::LINEARVELOCITY;
                    else
                        flags -= PhysicsDebugFlags::LINEARVELOCITY;
                }

                bool LINEARFORCE = flags & PhysicsDebugFlags::LINEARFORCE;
                if (ImGui::Checkbox("Linear Force", &LINEARFORCE)) {
                    if (LINEARFORCE)
                        flags += PhysicsDebugFlags::LINEARFORCE;
                    else
                        flags -= PhysicsDebugFlags::LINEARFORCE;
                }

                bool BROADPHASE = flags & PhysicsDebugFlags::BROADPHASE;
                if (ImGui::Checkbox("Broadphase", &BROADPHASE)) {
                    if (BROADPHASE)
                        flags += PhysicsDebugFlags::BROADPHASE;
                    else
                        flags -= PhysicsDebugFlags::BROADPHASE;
                }

                bool showPairs = flags & PhysicsDebugFlags::BROADPHASE_PAIRS;
                if (ImGui::Checkbox("Broadphase Pairs", &showPairs)) {
                    if (showPairs)
                        flags += PhysicsDebugFlags::BROADPHASE_PAIRS;
                    else
                        flags -= PhysicsDebugFlags::BROADPHASE_PAIRS;
                }

                physics3D->SetDebugDrawFlags(flags);
            }*/

            ImGui::EndPopup();
        }
    }

    ImGui::PopStyleColor();
    ImGui::Unindent();
}

#define DRAW_COMPONENT(ComponentType) drawComponentGizmos<ComponentType>(registry, camera, coord, offset, #ComponentType);

void SceneViewPanel::drawGizmos(entt::registry& registry, Camera& camera, const glm::vec2& coord, const glm::vec2& offset) {
    DRAW_COMPONENT(LightComponent);
    DRAW_COMPONENT(CameraComponent);
    //DRAW_COMPONENT(SoundComponent);
}
