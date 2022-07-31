#include "application_info_panel.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/filesystem/file_system.hpp"

using namespace fe;

ApplicationInfoPanel::ApplicationInfoPanel(Editor* editor) : EditorPanel{ICON_MDI_INFORMATION_VARIANT " Application Info###appinfo", "AppInfo", editor} {

}

ApplicationInfoPanel::~ApplicationInfoPanel() {

}

void ApplicationInfoPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin(title.c_str(), &active, flags);
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
                                       | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoSavedSettings;

        if (ImGui::TreeNode("Application")) {
            /*auto modules = Engine::Get()->getModules();

            if (ImGui::TreeNode("Modules")) {
                modules->onImGui();
                ImGui::TreePop();
            }*/

            if (ImGui::TreeNode("Asset Manager")) {
                auto manager = AssetRegistry::Get();

                if (!manager->getAssets().empty()) {
                    for (const auto& [id, type] : manager->getAssets()) {
                        std::string table{ "##" + std::to_string(id) };
                        if (ImGui::BeginTable(table.c_str(), 2, flags)) {
                            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableHeadersRow();

                            for (const auto& [path, asset] : type) {
                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(path.string().c_str());

                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("%ld", asset.use_count());
                            }
                            ImGui::EndTable();
                        }
                        ImGui::NewLine();
                    }
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Device Manager")) {
                auto manager = DeviceManager::Get();

                if (!manager->getWindows().empty()) {
                    ImGui::TextUnformatted("Windows");
                    if (ImGui::BeginTable("##windows", 11, flags)) {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Borderless", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Resizable", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Visible", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("VSync", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Iconified", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Focused", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Fullscreen", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Floating", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (const auto& [i, window]: enumerate(manager->getWindows())) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%lu", i);

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(window->getTitle().c_str());

                            ImGui::TableSetColumnIndex(2);
                            const auto& size = window->getSize();
                            ImGui::Text("[%d %d]", size.x, size.y);

                            ImGui::TableSetColumnIndex(3);
                            ImGui::SmallButton(window->isBorderless() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(4);
                            ImGui::SmallButton(window->isResizable() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(5);
                            ImGui::SmallButton(window->isVisible() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(6);
                            ImGui::SmallButton(window->isVSync() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(7);
                            ImGui::SmallButton(window->isIconified() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(8);
                            ImGui::SmallButton(window->isFocused() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(9);
                            ImGui::SmallButton(window->isFullscreen() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(10);
                            ImGui::SmallButton(window->isFloating() ? "Yes" : "No");
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }

                if (!manager->getMonitors().empty()) {
                    ImGui::TextUnformatted("Monitors");
                    if (ImGui::BeginTable("##monitors", 5, flags)) {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Primary", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Content Scale", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (const auto& [i, monitor]: enumerate(manager->getMonitors())) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%lu", i);

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(monitor->getName().c_str());

                            ImGui::TableSetColumnIndex(2);
                            bool primary = monitor->isPrimary();
                            if (primary) {
                                ImVec4 color{ 0.2f, 0.7f, 0.2f, 1.0f };
                                ImGui::PushStyleColor(ImGuiCol_Button, color);
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
                            }
                            ImGui::SmallButton(primary ? "Yes" : "No");
                            if (primary)
                                ImGui::PopStyleColor(3);

                            ImGui::TableSetColumnIndex(3);
                            auto size = monitor->getSize();
                            ImGui::Text("[%i %i]", size.x, size.y);

                            ImGui::TableSetColumnIndex(4);
                            auto scale = monitor->getContentScale();
                            ImGui::Text("[%f %f]", scale.x, scale.y);
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }

                if (!manager->getJoysticks().empty()) {
                    ImGui::TextUnformatted("Joysticks");
                    if (ImGui::BeginTable("##joysticks", 7, flags)) {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Port", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Connected", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Axis Count", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Button Count", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Hat Count", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (const auto& [i, joystick]: enumerate(manager->getJoysticks())) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%lu", i);

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%i", joystick->getPort());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(joystick->getName().c_str());

                            ImGui::TableSetColumnIndex(3);
                            bool connected = joystick->isConnected();
                            if (connected) {
                                ImVec4 color{ 0.2f, 0.7f, 0.2f, 1.0f };
                                ImGui::PushStyleColor(ImGuiCol_Button, color);
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));
                            }
                            ImGui::SmallButton(connected ? "Yes" : "No");
                            if (connected)
                                ImGui::PopStyleColor(3);

                            ImGui::TableSetColumnIndex(4);
                            ImGui::Text("%lu", joystick->getAxisCount());

                            ImGui::TableSetColumnIndex(5);
                            ImGui::Text("%lu", joystick->getButtonCount());

                            ImGui::TableSetColumnIndex(6);
                            ImGui::Text("%lu", joystick->getHatCount());
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }

                if (!manager->getCursors().empty()) {
                    ImGui::TextUnformatted("Cursors");
                    if (ImGui::BeginTable("##cursors", 4, flags)) {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Hotspot", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Standard", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (const auto& [i, cursor]: enumerate(manager->getCursors())) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%lu", i);

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", cursor->getPath().empty() ? "Empty" : cursor->getPath().string().c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(me::enum_name(cursor->getHotspot()).data());

                            ImGui::TableSetColumnIndex(3);
                            ImGui::TextUnformatted(me::enum_name(cursor->getStandard()).data());
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("File System")) {
                /*auto fs = FileSystem::Get();
                if (!fs->getSearchPath().empty()) {
                    if (ImGui::BeginTable("##search_paths", 1, flags)) {
                        ImGui::TableSetupColumn("Search Path", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (const auto& path: fs->getSearchPath()) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(path.string().c_str());
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }*/
                ImGui::TreePop();
            }

            /*if (ImGui::TreeNode("Virtual File System")) {
                auto vfs = VirtualFileSystem::Get();
                if (!vfs->getMounted().empty()) {
                    if (ImGui::BeginTable("##mounted_paths", 2, flags)) {
                        ImGui::TableSetupColumn("Virtual Path", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Physical Path", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (const auto& [virt, phys] : vfs->getMounted()) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(virt.string().c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(phys.string().c_str());
                        }
                        ImGui::EndTable();
                    }
                    ImGui::NewLine();
                }
                ImGui::TreePop();
            }*/

            ImGui::Text("FPS : %5.2i", Time::FramesPerSecond());
            ImGui::Text("Frame Time : %5.2f ms", Time::DeltaTime().asMilliseconds());
            //ImGui::NewLine();
            //ImGui::Text("Scene : %s", SceneManager::Get()->getCurrentScene()->getName().c_str());
            ImGui::TreePop();
        };
    }
    ImGui::End();
}
