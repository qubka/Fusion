#pragma once

#include "fusion/imgui/material_design_icons.hpp"
#include "fusion/imgui/imgui_utils.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imguizmo/ImGuizmo.h>

namespace fe {
    class Editor;
    class Scene;
    class EditorPanel {
        friend class Editor;
    public:
        EditorPanel(const std::string& title, const std::string& name, Editor* editor) : title{title}, name{name}, editor{editor} {};
        virtual ~EditorPanel() = default;

        const std::string& getTitle() const { return title; }
        const std::string& getName() const { return name; }

        bool& Active() { return active; }
        bool isActive() const { return active; }
        void setActive(bool flag) { active = flag; }

        Editor* getEditor() { return editor; }
        void setEditor(Editor* ptr) { editor = ptr; }

    protected:
        virtual void onImGui() = 0;
        virtual void onNewScene(Scene* scene) {}
        virtual void onNewProject() {}

        std::string title;
        std::string name;
        Editor* editor;
        bool active{ true };
    };
}
