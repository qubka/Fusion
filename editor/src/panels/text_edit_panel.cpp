#include "text_edit_panel.h"
#include "editor.h"

#include "fusion/devices/device_manager.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/filesystem/file_format.h"

using namespace fe;

TextEditPanel::TextEditPanel(fs::path filepath, std::function<void()> callback, Editor& editor)
        : path{std::move(filepath)}
        , callback{std::move(callback)}
        , EditorPanel{ICON_MDI_NOTE_TEXT "Text Editor###textedit", "TextEdit", editor}
{
    textEditor.SetCustomIdentifiers({});

    std::string extension{ FileSystem::GetExtension(path) };
    if (extension == "cpp") {
        const auto& lang = ImGui::TextEditor::LanguageDefinition::CPlusPlus();
        textEditor.SetLanguageDefinition(lang);
    } else if (extension == "glsl" || extension == "vert" || extension == "frag" || extension == "comp" || extension == "tesc" || extension == "tese" || extension == "geom") {
        const auto& lang = ImGui::TextEditor::LanguageDefinition::GLSL();
        textEditor.SetLanguageDefinition(lang);
    }

    textEditor.SetText(FileSystem::ReadText(path));
    textEditor.SetShowWhitespaces(false);
}

TextEditPanel::~TextEditPanel() {

}

void TextEditPanel::onImGui() {
    auto window = DeviceManager::Get()->getWindow(0);

    if (!!window->getKey(Key::LeftSuper) || (!!window->getKey(Key::LeftControl))) {
        if (!!window->getKey(Key::S)) {
            FileSystem::WriteText(path, textEditor.GetText());
        }
    }

    auto cpos = textEditor.GetCursorPosition();
    ImGui::SetWindowSize(ImVec2{800, 600}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin(title.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "CTRL+S")) {
                    FileSystem::WriteText(path, textEditor.GetText());
                    callback();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                bool ro = textEditor.IsReadOnly();
                if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                    textEditor.SetReadOnly(ro);
                ImGui::Separator();

                if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && textEditor.CanUndo()))
                    textEditor.Undo();
                if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, !ro && textEditor.CanRedo()))
                    textEditor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, textEditor.HasSelection()))
                    textEditor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl+X", nullptr, !ro && textEditor.HasSelection()))
                    textEditor.Cut();
                if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && textEditor.HasSelection()))
                    textEditor.Delete();
                if (ImGui::MenuItem("Paste", "Ctrl+V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    textEditor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem("Select all", nullptr, nullptr))
                    textEditor.SetSelection(ImGui::TextEditor::Coordinates(), ImGui::TextEditor::Coordinates(textEditor.GetTotalLines(), 0));

                if (ImGui::MenuItem("Close", nullptr, nullptr))
                    onClose();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Dark palette"))
                    textEditor.SetPalette(ImGui::TextEditor::GetDarkPalette());
                if (ImGui::MenuItem("Light palette"))
                    textEditor.SetPalette(ImGui::TextEditor::GetLightPalette());
                if (ImGui::MenuItem("Retro blue palette"))
                    textEditor.SetPalette(ImGui::TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1,
                    textEditor.GetTotalLines(), textEditor.IsOverwrite() ? "Ovr" : "Ins",
                    textEditor.CanUndo() ? "*" : " ", textEditor.GetLanguageDefinition().mName.c_str(),
                    path.filename().string().c_str());

        if (ImGui::IsItemActive()) {
            if (!!window->getKey(Key::LeftControl) && !!window->getKey(Key::S)) {
                FileSystem::WriteText(path, textEditor.GetText());
            }
        }

        textEditor.Render(name.c_str());
    }
    ImGui::End();
}

void TextEditPanel::onClose() {
    editor.removePanel(this);
}

void TextEditPanel::setErrors(const std::map<int, std::string>& errors) {
    textEditor.SetErrorMarkers(errors);
}