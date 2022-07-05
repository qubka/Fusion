#pragma once

namespace fe {
    class Editor;
    class Scene;

    class EditorPanel {
    public:
        EditorPanel() = default;
        virtual ~EditorPanel() = default;

        const std::string& getName() const { return name; }
        const std::string& getSimpleName() const { return simpleName; }

        virtual void OnImGui() = 0;

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
