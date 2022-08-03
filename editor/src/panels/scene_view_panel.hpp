#pragma once

#include "editor_panel.hpp"
#include "editor.hpp"

#include "fusion/scene/components.hpp"
#include "fusion/geometry/frustum.hpp"
#include "fusion/graphics/cameras/camera.hpp"

namespace fe {
    class SceneViewPanel : public EditorPanel {
    public:
        explicit SceneViewPanel(Editor* editor);
        ~SceneViewPanel() override;

        void onImGui() override;

    private:
        void drawGizmos(entt::registry& registry, Camera& camera, const glm::vec2& coord, const glm::vec2& offset);
        void drawToolBar();

        template<typename T>
        void drawComponentGizmos(entt::registry& registry, Camera& camera, const glm::vec2& coord, const glm::vec2& offset, const char* text) {
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
                    editor->getComponentIcon(type_id<T>, icon);

                    if (ImGui::Button(icon.c_str())) {
                        editor->setSelected(entity);
                    }

                    ImGui::PopStyleColor(3);

                    ImGuiUtils::Tooltip(text);
                }
            }
        }

        std::unordered_map<type_index, bool> showComponentGizmosMap;
    };
}
