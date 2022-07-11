#pragma once

#include "editor_panel.hpp"

#include <imgui-plugins/ImTextEditor.h>

namespace fe {
    class TextEditPanel : public EditorPanel {
    public:
        TextEditPanel(fs::path filename, std::function<void()>&& callback, Editor* editor);
        ~TextEditPanel() override;

        void onImGui() override;
        void onClose();

        void setOnSaveCallback(std::function<void()>&& func) { callback = std::move(func); }
        void setErrors(const std::map<int, std::string>& errors);

    private:
        TextEditor textEditor;
        fs::path filePath;
        std::function<void()> callback;
    };
}