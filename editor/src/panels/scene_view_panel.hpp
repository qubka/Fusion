#pragma once

#include "editor_panel.hpp"
#include "editor.hpp"

#include "fusion/scene/components.hpp"
#include "fusion/geometry/frustum.hpp"
#include "fusion/graphics/cameras/camera.hpp"

namespace fe {
    class SceneViewPanel : public EditorPanel {
    public:
        explicit SceneViewPanel(Editor& editor);
        ~SceneViewPanel() override;

        void onImGui() override;

    private:
        void drawToolBar();

        void drawGizmo(entt::registry& registry);
        void drawDebug(entt::registry& registry);
        // Return false when mouse is not hovering over gizmos
        bool drawComponent(entt::registry& registry, const glm::vec2& coord, const glm::vec2& offset);

        template<typename T>
        bool drawComponentGizmos(entt::registry& registry, Camera& camera, const glm::vec2& coord, const glm::vec2& offset, const char* text) {
            bool hovered = false;
            if (showComponentGizmosMap[type_id<T>]) {
                auto view = registry.view<T, TransformComponent>();
                for (const auto& [entity, component, transform] : view.each()) {
                    glm::vec3 pos = transform.getWorldPosition();
                    if (!camera.getFrustum().contains(pos))
                        continue;

                    float shift = ImGui::GetFontSize() * 0.5f;
                    glm::vec2 screenPos = camera.worldToScreen(pos, coord) + offset - shift;
                    ImGui::SetCursorPos(screenPos);

                    ImVec4 color{0.7f, 0.7f, 0.7f, 0.0f};
                    ImGui::PushStyleColor(ImGuiCol_Button, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));

                    std::string icon{ ICON_MDI_CUBE_OUTLINE };
                    editor.getComponentIcon(type_id<T>, icon);

                    if (ImGui::Button(icon.c_str())) {
                        editor.setSelected(entity);
                    }

                    hovered |= ImGui::IsItemHovered();

                    ImGui::PopStyleColor(3);

                    ImGuiUtils::Tooltip(text);
                }
            }
            return hovered;
        }

        fst::unordered_flatmap<type_index, bool> showComponentGizmosMap;
    };
}
