#include "editor_settings_panel.hpp"

using namespace fe;

EditorSettingsPanel::EditorSettingsPanel(Editor& editor) : EditorPanel{ICON_MDI_INFORMATION_VARIANT " Editor Settings###editorsettings", "Editor Settings", editor} {

}

EditorSettingsPanel::~EditorSettingsPanel() {

}

void EditorSettingsPanel::onImGui() {

}
