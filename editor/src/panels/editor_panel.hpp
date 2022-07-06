#pragma once

#include "fusion/imgui/material_design_icons.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace fe {
    class Editor;
    class Scene;

    class EditorPanel {
    public:
        EditorPanel(std::string name, std::string simpleName) : name{std::move(name)}, simpleName{std::move(simpleName)} {};
        virtual ~EditorPanel() = default;

        const std::string& getName() const { return name; }
        const std::string& getSimpleName() const { return simpleName; }

        virtual void onImGui() = 0;

        virtual void onNewScene(Scene* scene) {
        }

        virtual void onNewProject() {
        }

        virtual void onRender() {
        }

        bool isEnabled() const { return enabled; }
        void setEnabled(bool enable) { this->enabled = enable; }

        Editor* getEditor() { return editor; }
        void setEditor(Editor* editor) { this->editor = editor; }

    protected:
        std::string name;
        std::string simpleName;
        Editor* editor{ nullptr };
        bool enabled{ true };
    };
}
