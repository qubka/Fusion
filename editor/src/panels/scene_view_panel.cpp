#include "scene_view_panel.hpp"
#include "editor.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/device_manager.hpp"

using namespace fe;

SceneViewPanel::SceneViewPanel(Editor* editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Scene###scene", "Scene", editor} {
    //showComponentGizmoMap[typeid(LightComponent)] = true;
    //showComponentGizmoMap[typeid(CameraComponent)] = true;
    //showComponentGizmoMap[typeid(SoundComponent).] = true;
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

    auto camera = editor->getCamera();
    if (!camera) {
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }

    ImVec2 offset = ImGui::GetCursorPos(); // Usually ImVec2{0.0f, 50.0f};

    ImGuizmo::SetDrawlist();
    auto sceneViewSize = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() - offset * 0.5f; // - offset * 0.5f;
    auto sceneViewPosition = ImGui::GetWindowPos() + offset;

    sceneViewSize.x -= static_cast<int>(sceneViewSize.x) % 2 != 0 ? 1.0f : 0.0f;
    sceneViewSize.y -= static_cast<int>(sceneViewSize.y) % 2 != 0 ? 1.0f : 0.0f;

    float aspect = static_cast<float>(sceneViewSize.x) / static_cast<float>(sceneViewSize.y);

    if (glm::epsilonEqual(aspect, camera->getAspectRatio(), FLT_EPSILON)) {
        camera->setAspectRatio(aspect);
    }

    editor->setSceneViewPanelPosition({sceneViewPosition.x, sceneViewPosition.y});

    bool halfRes = editor->getSettings().halfRes;

    if (halfRes)
        sceneViewSize *= 0.5f;

    //resize({sceneViewSize.x, sceneViewSize.y});

    if (halfRes)
        sceneViewSize *= 2.0f;

    static uint32_t id = 1; // custom image
    ImGuiUtils::Image(&id, glm::vec2{ sceneViewSize.x, sceneViewSize.y }, true);

    auto windowSize = ImGui::GetWindowSize();

    ImVec2& minBound = sceneViewPosition;
    ImVec2  maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };

    bool updateCamera = ImGui::IsMouseHoveringRect(minBound, maxBound); // || Input::Get().GetMouseMode() == MouseMode::Captured;

    editor->setSceneActive(ImGui::IsWindowFocused() && !ImGuizmo::IsUsing() && updateCamera);

    ImGuizmo::SetRect(sceneViewPosition.x, sceneViewPosition.y, sceneViewSize.x, sceneViewSize.y);

    editor->setSceneViewActive(updateCamera);

    ImGui::GetWindowDrawList()->PushClipRect(sceneViewPosition, { sceneViewSize.x + sceneViewPosition.x, sceneViewSize.y + sceneViewPosition.y - 2.0f });

    editor->onImGuizmo();

    //auto window = DeviceManager::Get()->getWindow(0);

    /*if (updateCamera && editor->isSceneActive() && !ImGuizmo::IsUsing() && !!window->getMouseButton(MouseButton::ButtonLeft)) {
        auto clickPos = window->getMousePosition() - glm::vec2{sceneViewPosition.x / dpi, sceneViewPosition.y / dpi};

        Ray ray = editor->getScreenRay(int(clickPos.x), int(clickPos.y), camera, int(sceneViewSize.x / dpi), int(sceneViewSize.y / dpi));
        editor->selectObject(ray);
    }*/

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            editor->fileOpenCallback(reinterpret_cast<const char*>(payload->Data));
        }
        ImGui::EndDragDropTarget();
    }

    drawGizmos(scene->getRegistry(), *camera, {sceneViewSize.x, sceneViewSize.y}, {offset.x, offset.y});

    ImGui::PopStyleVar();
    ImGui::End();
}

void SceneViewPanel::drawToolBar() {
    ImGui::Indent();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
    bool selected = false;

    {
        selected = editor->getSettings().gizmosOperation == 4; // TODO:
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_CURSOR_DEFAULT))
            editor->getSettings().gizmosOperation = 4;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Select");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::TRANSLATE;
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_ARROW_ALL))
            editor->getSettings().gizmosOperation = ImGuizmo::TRANSLATE;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Translate");
    }

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::ROTATE;
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_ROTATE_ORBIT))
            editor->getSettings().gizmosOperation = ImGuizmo::ROTATE;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Rotate");
    }

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::SCALE;
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_ARROW_EXPAND_ALL))
            editor->getSettings().gizmosOperation = ImGuizmo::SCALE;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Scale");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::UNIVERSAL;
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_CROP_ROTATE))
            editor->getSettings().gizmosOperation = ImGuizmo::UNIVERSAL;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Universal");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    {
        selected = editor->getSettings().gizmosOperation == ImGuizmo::BOUNDS;
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_BORDER_NONE))
            editor->getSettings().gizmosOperation = ImGuizmo::BOUNDS;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Bounds");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    ImGui::SameLine();
    {
        selected = (editor->getSettings().snapGizmos == true);

        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

        if (ImGui::Button(ICON_MDI_MAGNET))
            editor->getSettings().snapGizmos = !selected;

        if (selected)
            ImGui::PopStyleColor();
        ImGuiUtils::Tooltip("Snap");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    if (ImGui::Button("Gizmos " ICON_MDI_CHEVRON_DOWN))
        ImGui::OpenPopup("GizmosPopup");
    if (ImGui::BeginPopup("GizmosPopup")) {
        {
            //ImGui::Checkbox("Grid", &m_Editor->ShowGrid());
           // ImGui::Checkbox("Selected Gizmos", &m_Editor->ShowGizmos());
            //ImGui::Checkbox("View Selected", &m_Editor->ShowViewSelected());

            ImGui::Separator();
            ImGui::Checkbox("Camera", &showComponentGizmosMap[typeid(CameraComponent)]);
            //ImGui::Checkbox("Light", &showComponentGizmosMap[typeid(LightComponent)]);
            //ImGui::Checkbox("Audio", &showComponentGizmosMap[typeid(SoundComponent)]);

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

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    // Editor Camera Settings

    auto& camera = *editor->getCamera();
    bool ortho = camera.isOrthographic();

    selected = !ortho;
    if (selected)
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
    if (ImGui::Button(ICON_MDI_AXIS_ARROW " 3D")) {
        if (ortho) {

        }
    }

    if (selected)
        ImGui::PopStyleColor();
    ImGui::SameLine();

    selected = ortho;
    if (selected)
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
    if (ImGui::Button(ICON_MDI_ANGLE_RIGHT "2D")) {
        if (!ortho) {
            // TODO:: Switch camera
        }
    }
    if (selected)
        ImGui::PopStyleColor();

    ImGui::PopStyleColor();
    ImGui::Unindent();
}

void SceneViewPanel::drawGizmos(entt::registry& registry, Camera& camera, const glm::vec2& coord, const glm::vec2& offset) {
    //showComponentGizmos<LightComponent>(registry, camera, coord, offset);
    showComponentGizmos<CameraComponent>(registry, camera, coord, offset);
    //showComponentGizmos<SoundComponent>(registry, camera, coord, offset);
}
