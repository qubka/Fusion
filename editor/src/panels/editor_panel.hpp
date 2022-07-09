#pragma once

#include "fusion/imgui/material_design_icons.hpp"
#include "fusion/imgui/imgui_utils.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace fe {
    class Editor;
    class Scene;
    class EditorPanel {
    public:
        EditorPanel(std::string name, std::string simpleName, Editor* editor) : name{std::move(name)}, simpleName{std::move(simpleName)}, editor{editor} {};
        virtual ~EditorPanel() = default;

        const std::string& getName() const { return name; }
        const std::string& getSimpleName() const { return simpleName; }

        virtual void onImGui() = 0;
        virtual void onNewScene(Scene* scene) {}
        virtual void onNewProject() {}

        bool& Enabled() { return enabled; }
        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) { enabled = flag; }

        Editor* getEditor() { return editor; }
        void setEditor(Editor* ptr) { editor = ptr; }

    protected:
        std::string name;
        std::string simpleName;
        Editor* editor;
        bool enabled{ true };
    };
}
