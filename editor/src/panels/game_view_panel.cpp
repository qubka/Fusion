#include "game_view_panel.h"
#include "editor.h"

#include "fusion/graphics/graphics.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/scene/components/camera_component.h"

using namespace fe;

GameViewPanel::GameViewPanel(Editor& editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Game###game", "Game", editor} {

}

GameViewPanel::~GameViewPanel() {

}

void GameViewPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

    auto scene = SceneManager::Get()->getScene();
    if(!ImGui::Begin(title.c_str(), &active, flags) || !scene) {
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

    auto camera = scene->getComponent<CameraComponent>();
    if (!camera) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(viewportPos, viewportPos + viewportSize, IM_COL32(0, 0, 0, 255));
        ImGuiUtils::TextCentered(ICON_MDI_CAMERA_OFF " No Cameras Rendering", 0.0f);
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }

    if (!editor.getSettings().freeAspect) {
        float fixedAspect = editor.getSettings().fixedAspect;
        float heightNeededForAspect = viewportSize.x / fixedAspect;

        if (heightNeededForAspect > viewportSize.y) {
            viewportSize.x = viewportSize.y * fixedAspect;
            float xOffset = (ImGui::GetContentRegionAvail() - viewportSize).x * 0.5f;
            viewportPos.x += xOffset;
            ImGui::SetCursorPos({ xOffset, ImGui::GetCursorPosY() + viewportOffset.x });
            viewportOffset.x += xOffset;
        } else {
            viewportSize.y = viewportSize.x / fixedAspect;
            float yOffset = (ImGui::GetContentRegionAvail() - viewportSize).y * 0.5f;
            viewportPos.y += yOffset;

            ImGui::SetCursorPos({ ImGui::GetCursorPosX(), yOffset + viewportOffset.y });
            viewportOffset.y += yOffset;
        }
    }

    float aspect = viewportSize.x / viewportSize.y;
    camera->setAspectRatio(aspect);

    auto renderStage = Graphics::Get()->getRenderStage(1);
    if (!renderStage->setViewport({glm::vec2{1.0f, 1.0f}, glm::uvec2{viewportSize.x, viewportSize.y}, glm::ivec2{0, 0}})) {
        ImGuiUtils::Image((Texture2d*)renderStage->getDescriptor("game_image"), viewportSize, true);
    }

    ImVec2& minBound = viewportPos;
    ImVec2  maxBound{ minBound + ImGui::GetWindowSize() };

    bool viewportHovered = ImGui::IsMouseHoveringRect(minBound, maxBound); // || Input::Get().getMouseMode() == MouseMode::Captured;
    bool viewportFocused = ImGui::IsWindowFocused();

    editor.setSceneActive(viewportFocused && viewportHovered && !ImGuizmo::IsUsing());

    if (editor.getSettings().showStats) {
        static bool open = true;
        static int corner = 0;
        const float distance = 5.0f;

        if (corner != -1) {
            ImVec2 windowPos{
                    (corner & 1) ? (viewportPos.x + viewportSize.x - distance) : (viewportPos.x + distance),
                    (corner & 2) ? (viewportPos.y + viewportSize.y - distance) : (viewportPos.y + distance)
            };
            ImVec2 windowPosPivot{(corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f};
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
        }

        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("Stats overlay", &open,
                         (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDocking |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav)) {
            ImGui::Text("%.2f ms (%.i FPS)", Time::DeltaTime().asMilliseconds(), Time::FramesPerSecond());
            ImGui::Separator();

            if (ImGui::IsMousePosValid()) {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            } else
                ImGui::TextUnformatted("Mouse Position: <invalid>");

            //ImGui::Text("Num Rendered Objects %u", frameStats.NumRenderedObjects);
            //ImGui::Text("Vertices %lu", frameStats.totalVertices);
            //ImGui::Text("Draw Calls  %lu", frameStats.drawCalls);
            //ImGui::Text("Used GPU Memory : %.1f mb | Total : %.1f mb", frameStats.UsedGPUMemory, frameStats.TotalGPUMemory);

            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("Custom", nullptr, corner == -1))
                    corner = -1;
                if (ImGui::MenuItem("Top-left", nullptr, corner == 0))
                    corner = 0;
                if (ImGui::MenuItem("Top-right", nullptr, corner == 1))
                    corner = 1;
                if (ImGui::MenuItem("Bottom-left", nullptr, corner == 2))
                    corner = 2;
                if (ImGui::MenuItem("Bottom-right", nullptr, corner == 3))
                    corner = 3;
                if (open && ImGui::MenuItem("Close"))
                    open = false;
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void GameViewPanel::drawToolBar() {
    ImGui::Indent();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
    bool selected = false;

    float xAvail = ImGui::GetContentRegionAvail().x - 285.0f;
    {
        selected = editor.getSettings().showStats;
        ImGui::SameLine(xAvail);
        if (ImGuiUtils::ToggleButton(ICON_MDI_POLL_BOX " Stats", selected))
            editor.getSettings().showStats = selected;
        ImGuiUtils::Tooltip("Show Statistics");
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    if (ImGui::Button(ICON_MDI_ASPECT_RATIO " Aspect " ICON_MDI_CHEVRON_DOWN))
        ImGui::OpenPopup("AspectPopup");
    if (ImGui::BeginPopup("AspectPopup")) {

        static const std::vector<std::pair<std::string_view, float>> SUPPORTED_ASPECTS = {{
                    {"Free Aspect ", 1.0f},
                    {"16:10", 16.0f / 10.0f},
                    {"16:9", 16.0f / 9.0f},
                    {"4:3", 4.0f / 3.0f},
                    {"3:2", 4.0f / 2.0f},
                    {"9:16", 9.0f / 16.0f}
        }};

        float currentAspect = editor.getSettings().fixedAspect;
        auto it = std::find_if(SUPPORTED_ASPECTS.begin(), SUPPORTED_ASPECTS.end(), [currentAspect](const auto& p) {
            return glm::epsilonEqual(p.second, currentAspect, FLT_EPSILON);
        });

        for (const auto& [name, aspect] : SUPPORTED_ASPECTS) {
            bool is_selected = (it->first == name);
            if (ImGui::Checkbox(name.data(), &is_selected)) {
                editor.getSettings().freeAspect = (name == "Free Aspect ");
                editor.getSettings().fixedAspect = aspect;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    {
        selected = editor.getSettings().muteAudio;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_VOLUME_MUTE " Mute", selected))
            editor.getSettings().muteAudio = selected;
        ImGuiUtils::Tooltip("Mute audio");
    }

    ImGui::SameLine();

    {
        selected = editor.getSettings().fullScreenOnPlay;
        ImGui::SameLine();
        if (ImGuiUtils::ToggleButton(ICON_MDI_WINDOW_MAXIMIZE " Maximise", selected))
            editor.getSettings().fullScreenOnPlay = selected;
        ImGuiUtils::Tooltip("Maximise on play");
    }

    ImGui::PopStyleColor();
    ImGui::Unindent();
}
