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
        EditorPanel(std::string title, std::string name, Editor* editor) : title{std::move(title)}, name{std::move(name)}, editor{editor} {};
        virtual ~EditorPanel() = default;

        const std::string& getTitle() const { return title; }
        const std::string& getName() const { return name; }

        virtual void onImGui() = 0;
        virtual void onNewScene(Scene* scene) {}
        virtual void onNewProject() {}

        bool& Enabled() { return enabled; }
        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) { enabled = flag; }

        Editor* getEditor() { return editor; }
        void setEditor(Editor* ptr) { editor = ptr; }

    protected:
        std::string title;
        std::string name;
        Editor* editor;
        bool enabled{ true };
    };
}
