#pragma once

#include "editor_panel.h"

#include <imgui-plugins/ImTextEditor.h>

namespace fe {
    class TextEditPanel : public EditorPanel {
    public:
        TextEditPanel(fs::path filepath, std::function<void()> callback, Editor& editor);
        ~TextEditPanel() override;

        void onImGui() override;
        void onClose();

        void setOnSaveCallback(const std::function<void()>& func) { callback = func; }
        void setErrors(const std::map<int, std::string>& errors);

    private:
        ImGui::TextEditor textEditor;
        fs::path path;
        std::function<void()> callback;
    };
}