#include "text_edit_panel.hpp"

#include <utility>
#include "editor.hpp"

#include "fusion/filesystem/file_system.hpp"
#include "fusion/devices/device_manager.hpp"

using namespace fe;

TextEditPanel::TextEditPanel(fs::path filepath, std::function<void()>&& callback, Editor* editor)
    : filePath{std::move(filepath)}
    , callback{std::move(callback)}
    , EditorPanel{ICON_MDI_NOTE_TEXT "Text Editor###textedit", "TextEdit", editor}
{
    textEditor.SetCustomIdentifiers({});

    auto extension = FileSystem::GetExtension(filePath);

    /*if (extension == "lua" || extension == "Lua") {
        auto lang = TextEditor::LanguageDefinition::Lua();
        textEditor.SetLanguageDefinition(lang);

        auto& customIdentifiers = LuaManager::GetIdentifiers();
        TextEditor::Identifiers identifiers;

        for (auto& k: customIdentifiers) {
            TextEditor::Identifier id;
            id.mDeclaration = "Engine function";
            identifiers.insert(std::make_pair(k, id));
        }

        textEditor.SetCustomIdentifiers(identifiers);
    }*/ if (extension == "cpp") {
        auto lang = TextEditor::LanguageDefinition::CPlusPlus();
        textEditor.SetLanguageDefinition(lang);
    } else if (extension == "glsl" || extension == "vert" || extension == "frag") {
        auto lang = TextEditor::LanguageDefinition::GLSL();
        textEditor.SetLanguageDefinition(lang);
    }

    auto string = FileSystem::ReadText(filePath);
    textEditor.SetText(string);
    textEditor.SetShowWhitespaces(false);
}

TextEditPanel::~TextEditPanel() {

}

void TextEditPanel::onImGui() {
    auto window = DeviceManager::Get()->getWindow(0);

    if (!!window->getKey(Key::LeftSuper) || (!!window->getKey(Key::LeftControl))) {
        if (!!window->getKey(Key::S)) {
            auto textToSave = textEditor.GetText();
            FileSystem::Write(filePath, textToSave.data(), textToSave.length());
        }
    }

    auto cpos = textEditor.GetCursorPosition();
    ImGui::SetWindowSize(ImVec2{800, 600}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin(name.c_str(), &enabled, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "CTRL+S")) {
                    auto textToSave = textEditor.GetText();
                    FileSystem::Write(filePath, textToSave.data(), textToSave.length());
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
                if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && textEditor.CanRedo()))
                    textEditor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, textEditor.HasSelection()))
                    textEditor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && textEditor.HasSelection()))
                    textEditor.Cut();
                if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && textEditor.HasSelection()))
                    textEditor.Delete();
                if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    textEditor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem("Select all", nullptr, nullptr))
                    textEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(textEditor.GetTotalLines(), 0));

                if (ImGui::MenuItem("Close", nullptr, nullptr))
                    onClose();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Dark palette"))
                    textEditor.SetPalette(TextEditor::GetDarkPalette());
                if (ImGui::MenuItem("Light palette"))
                    textEditor.SetPalette(TextEditor::GetLightPalette());
                if (ImGui::MenuItem("Retro blue palette"))
                    textEditor.SetPalette(TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1,
                    textEditor.GetTotalLines(), textEditor.IsOverwrite() ? "Ovr" : "Ins",
                    textEditor.CanUndo() ? "*" : " ", textEditor.GetLanguageDefinition().mName.c_str(),
                    filePath.filename().c_str());

        if (ImGui::IsItemActive()) {
            if (!!window->getKey(Key::LeftControl) && !!window->getKey(Key::S)) {
                auto textToSave = textEditor.GetText();
                FileSystem::Write(filePath, textToSave.data(), textToSave.length());
            }
        }

        textEditor.Render(name.c_str());
    }
    ImGui::End();
}

void TextEditPanel::onClose() {
    editor->removePanel(this);
}

void TextEditPanel::setErrors(const std::map<int, std::string>& errors) {
    textEditor.SetErrorMarkers(errors);
}