#pragma once

#include <map>
#include <set>
#include <functional>
#include <string>

#include "material_design_icons.hpp"
#include "imgui_utils.hpp"

#include <entt/entt.hpp>
#include <imgui/imgui.h>

/** @link https://github.com/Green-Sky/imgui_entt_entity_editor_demo **/
namespace ImGui {
    template<class Component, class EntityType>
    void ComponentEditorWidget(entt::basic_registry<EntityType>& registry, EntityType entity) {
    }

    template<class Component, class EntityType>
    void ComponentAddAction(entt::basic_registry<EntityType>& registry, EntityType entity) {
        registry.template emplace<Component>(entity);
    }

    template<class Component, class EntityType>
    void ComponentRemoveAction(entt::basic_registry<EntityType>& registry, EntityType entity) {
        registry.template remove<Component>(entity);
    }

    template<typename EntityType>
    class EntityEditor {
    private:
        using Registry = entt::basic_registry<EntityType>;

        struct ComponentInfo {
            using Callback = std::function<void(Registry&, EntityType)>;
            std::string name;
            Callback widget, create, destroy;
        };

    private:
        using ComponentTypeID = ENTT_ID_TYPE;
        ImGuiTextFilter componentFilter;

        std::map<ComponentTypeID, ComponentInfo> componentInfos;

        bool entityHasComponent(Registry& registry, EntityType& entity, ComponentTypeID type_id) {
            const auto storage_it = registry.storage(type_id);
            return storage_it != registry.storage().end() && storage_it->second.contains(entity);
        }

    public:
        template<class Component>
        ComponentInfo& registerComponent(const ComponentInfo& info) {
            auto index = entt::type_hash<Component>::value();
            auto [it, result] = componentInfos.insert_or_assign(index, info);
            assert(result);
            return std::get<ComponentInfo>(*it);
        }

        template<class Component>
        ComponentInfo& registerComponent(const std::string& name, typename ComponentInfo::Callback widget) {
            return registerComponent<Component>(ComponentInfo{
                    name,
                    widget,
                    ComponentAddAction<Component, EntityType>,
                    ComponentRemoveAction<Component, EntityType>,
            });
        }

        template<class Component>
        ComponentInfo& registerComponent(const std::string& name) {
            return registerComponent<Component>(name, ComponentEditorWidget<Component, EntityType>);
        }

        // calls all the ImGui functions
        // call this every frame
        void Render(Registry& registry, EntityType& entity) {
            if (entity != entt::null) {
                ImGuiStyle& style = ImGui::GetStyle();
                std::map<ComponentTypeID, ComponentInfo> hasNot;
                for (auto& [component, info] : componentInfos) {
                    if (entityHasComponent(registry, entity, component)) {
                        ImGui::PushID(component);

                        const auto& label = info.name;

                        bool open = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
                        bool removed = false;

                        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() - style.ItemSpacing.x);

                        ImVec4 color{ 0.7f, 0.7f, 0.7f, 0.0f };
                        ImGui::PushStyleColor(ImGuiCol_Button, color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGuiColorScheme::Hovered(color));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGuiColorScheme::Active(color));

                        if (ImGui::Button((ICON_MDI_TUNE "##" + label).c_str()))
                            ImGui::OpenPopup(("Remove Component" + label).c_str());

                        ImGui::PopStyleColor(3);

                        if (ImGui::BeginPopup(("Remove Component" + label).c_str(), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
                            if (ImGui::Selectable(("Remove##" + label).c_str())) {
                                info.destroy(registry, entity);
                                removed = true;
                            }
                            ImGui::EndPopup();
                            if (removed) {
                                ImGui::PopID();
                                continue;
                            }
                        }
                        if (open) {
                            ImGui::PushID("Widget");
                            info.widget(registry, entity);
                            ImGui::PopID();
                        }
                        ImGui::PopID();
                    } else {
                        hasNot[component] = info;
                    }
                }

                if (!hasNot.empty()) {
                    if (ImGui::Button(ICON_MDI_PLUS_BOX_OUTLINE " Add Component", ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f })) {
                        ImGui::OpenPopup("addComponent");
                    }

                    if (ImGui::BeginPopup("addComponent", ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Dummy({200.0f, 0.0f}); // fix resize
                        ImGui::Separator();

                        ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
                        ImGui::SameLine();

                        float filterSize = ImGui::GetContentRegionAvail().x - style.IndentSpacing;
                        filterSize = std::min(200.0f, filterSize);
                        componentFilter.Draw("##ComponentFilter", filterSize);

                        for (auto& [component, info] : hasNot) {
                            bool show = true;
                            if (componentFilter.IsActive()) {
                                if (!componentFilter.PassFilter(info.name.c_str())) {
                                    show = false;
                                }
                            }
                            if (show) {
                                ImGui::PushID(component);
                                if (ImGui::Selectable(info.name.c_str())) {
                                    info.create(registry, entity);
                                }
                                ImGui::PopID();
                            }
                        }

                        ImGui::EndPopup();
                    }
                }
            }
        }
    };
}
