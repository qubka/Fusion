#include "hierarchy_panel.hpp"
#include "editor.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/systems/hierarchy_system.hpp"

using namespace fe;

HierarchyPanel::HierarchyPanel(Editor* editor) : EditorPanel{ICON_MDI_FILE_TREE " Hierarchy###hierarchy", "Hierarchy", editor} {

}

HierarchyPanel::~HierarchyPanel() {

}

void HierarchyPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    currentPrevious = entt::null;
    auto window = DeviceManager::Get()->getWindow(0);
    selectUp = !!window->getKey(Key::Up);
    selectDown = !!window->getKey(Key::Down);

    ImGui::Begin(title.c_str(), &enabled, flags);
    {
        auto scene = SceneManager::Get()->getScene();
        if (!scene) {
            ImGui::End();
            return;
        }

        auto hierarchySystem = scene->getSystem<HierarchySystem>();
        if (!hierarchySystem || !hierarchySystem->isEnabled()) {
            ImGui::End();
            return;
        }

        auto& registry = scene->getRegistry();

        auto addEntity = [scene]() {
            if (ImGui::Selectable("Add Empty Entity")) {
                scene->createEntity();
            }

            /*if (ImGui::Selectable("Add Light")) {
                auto entity = scene->CreateEntity("Light");
                entity.AddComponent<Graphics::Light>();
                entity.GetOrAddComponent<Maths::Transform>();
            }

            if (ImGui::Selectable("Add Rigid Body")) {
                auto entity = scene->CreateEntity("RigidBody");
                entity.AddComponent<RigidBody3DComponent>();
                entity.GetOrAddComponent<Maths::Transform>();
                entity.AddComponent<AxisConstraintComponent>(entity, Axes::XZ);
                entity.GetComponent<RigidBody3DComponent>().GetRigidBody()->SetCollisionShape(
                        CollisionShapeType::CollisionCuboid);
            }

            if (ImGui::Selectable("Add Camera")) {
                auto entity = scene->CreateEntity("Camera");
                entity.AddComponent<Camera>();
                entity.GetOrAddComponent<Maths::Transform>();
            }

            if (ImGui::Selectable("Add Sprite")) {
                auto entity = scene->CreateEntity("Sprite");
                entity.AddComponent<Graphics::Sprite>();
                entity.GetOrAddComponent<Maths::Transform>();
            }

            if (ImGui::Selectable("Add Lua Script")) {
                auto entity = scene->CreateEntity("LuaScript");
                entity.AddComponent<LuaScriptComponent>();
            }*/
        };

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));

        if (ImGui::Button(ICON_MDI_PLUS)) {
            // Add Entity Menu
            ImGui::OpenPopup("AddEntity");
        }

        if (ImGui::BeginPopup("AddEntity")) {
            addEntity();
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
        ImGui::SameLine();

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        {
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
            hierarchyFilter.Draw("##HierarchyFilter", ImGui::GetContentRegionAvail().x - style.IndentSpacing);
            ImGuiUtils::DrawItemActivityOutline(2.0f, false);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }

        if (!hierarchyFilter.IsActive()) {
            ImGui::SameLine();
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::SetCursorPosX(ImGui::GetFontSize() * 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, style.FramePadding.y });
            ImGui::TextUnformatted("Search...");
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }

        ImGui::PopStyleColor();
        ImGui::Unindent();

        ImGui::Separator();

        ImGui::BeginChild("##hierarchy_nodes");
        {
            if (ImGui::BeginPopupContextWindow()) {
                auto copiedEntity = editor->getCopiedEntity();
                if (copiedEntity != entt::null && registry.valid(copiedEntity)) {
                    if (ImGui::Selectable("Paste")) {
                        if (!registry.valid(copiedEntity)) {
                            editor->setCopiedEntity(entt::null);
                        } else {
                            scene->duplicateEntity(copiedEntity);

                            if (editor->getCutCopyEntity()) {
                                hierarchySystem->destroyParent(editor->getCopiedEntity());
                            }
                        }
                    }
                } else {
                    ImGui::TextDisabled("Paste");
                }

                ImGui::Separator();

                addEntity();

                ImGui::EndPopup();
            }

            {
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                        auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
                        if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
                            hierarchySystem->reparent(entity, entt::null, *hierarchy);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Indent();

                //ImGuiUtils::AlternatingRowsBackground(ImGui::GetFontSize() + style.FramePadding.y);

                registry.each([&](auto entity) {
                    if (registry.valid(entity)) {
                        auto hierarchy = registry.try_get<HierarchyComponent>(entity);
                        if (!hierarchy || hierarchy->parent == entt::null)
                            drawNode(entity, registry);
                    }
                });

                // Only supports one scene
                if (const ImGuiPayload* payload = ImGui::GetDragDropPayload(); payload && payload->IsDataType("SCENE_HIERARCHY_ITEM")) {
                    bool acceptable = false;
                    
                    auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
                    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
                        acceptable = hierarchy->parent != entt::null;
                    }

                    if (acceptable) {
                        ImVec2 minSpace = ImGui::GetWindowContentRegionMin();
                        ImVec2 maxSpace = ImGui::GetWindowContentRegionMax();

                        float yScroll = ImGui::GetScrollY();
                        float yOffset = std::max(45.0f, yScroll); // Dont include search bar
                        minSpace.x += ImGui::GetWindowPos().x + 1.0f;
                        minSpace.y += ImGui::GetWindowPos().y + 1.0f + yOffset;
                        maxSpace.x += ImGui::GetWindowPos().x - 1.0f;
                        maxSpace.y += ImGui::GetWindowPos().y - 1.0f + yScroll;

                        if (ImGui::BeginDragDropTargetCustom(ImRect{ minSpace, maxSpace }, ImGui::GetID("Panel Hierarchy"))) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                                auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
                                if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
                                    hierarchySystem->reparent(entity, entt::null, *hierarchy);
                                    registry.remove<HierarchyComponent>(entity);
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }
                }
            }

            ImGui::EndChild();
        }

        ImGui::End();
    }
}

void HierarchyPanel::drawNode(entt::entity node, entt::registry& registry) {
    bool show = true;

    if (!registry.valid(node))
        return;

    const auto nameComponent = registry.try_get<NameComponent>(node);
    std::string name = nameComponent ? *nameComponent : std::to_string(entt::to_integral(node));

    if (hierarchyFilter.IsActive()) {
        if (!hierarchyFilter.PassFilter(name.c_str())) {
            show = false;
        }
    }

    if (show) {
        ImGui::PushID(static_cast<int>(node));
        auto hierarchy = registry.try_get<HierarchyComponent>(node);
        bool noChildren = true;

        if (hierarchy != nullptr && hierarchy->first != entt::null)
            noChildren = false;

        ImGuiTreeNodeFlags nodeFlags = ((editor->getSelected() == node) ? ImGuiTreeNodeFlags_Selected : 0);

        nodeFlags |=
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

        if (noChildren) {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;
        }

        auto activeComponent = registry.try_get<ActiveComponent>(node);
        bool active = activeComponent ? *activeComponent : true;

        if (!active)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

        bool isDoubleClicked = false;
        if (node == doubleClicked) {
            isDoubleClicked = true;
        }

        if (isDoubleClicked)
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 1.0f, 2.0f });

        if (hadRecentDroppedEntity == node) {
            ImGui::SetNextItemOpen(true);
            hadRecentDroppedEntity = entt::null;
        }

        std::string icon = ICON_MDI_CUBE_OUTLINE;

        if (registry.any_of<CameraComponent>(node)) {
            editor->getComponentIcon(typeid(CameraComponent), icon);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtils::GetIconColor());
        // ImGui::BeginGroup();
        bool nodeOpen = ImGui::TreeNodeEx((void*) (intptr_t) entt::to_integral(node), nodeFlags, "%s", icon.c_str());
        {
            // Allow clicking of icon and text. Need twice as they are separated
            if (ImGui::IsItemClicked())
                editor->setSelected(node);
            else if (doubleClicked == node && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered(ImGuiHoveredFlags_None))
                doubleClicked = entt::null;
        }

        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (!isDoubleClicked)
            ImGui::TextUnformatted(name.c_str());
        // ImGui::EndGroup();

        if (isDoubleClicked) {
            static char objName[256];
            std::strcpy(objName, name.c_str());
            ImGui::PushItemWidth(-1);
            if (ImGui::InputText("##EntityName", objName, IM_ARRAYSIZE(objName), 0))
                registry.get_or_emplace<NameComponent>(node).name = objName;
            ImGui::PopStyleVar();
        }

#if 0
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 22.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.7f, 0.7f, 0.7f, 0.0f});
            if (ImGui::Button(active ? ICON_MDI_EYE : ICON_MDI_EYE_OFF)) {
                auto& activeComponent = registry.get_or_emplace<ActiveComponent>(node);
                activeComponent.active = !active;
            }
            ImGui::PopStyleColor();
#endif
        auto scene = SceneManager::Get()->getScene();
        auto hierarchySystem = scene->getSystem<HierarchySystem>();

        if (!active)
            ImGui::PopStyleColor();

        bool deleteEntity = false;
        if (ImGui::BeginPopupContextItem(name.c_str())) {
            if (ImGui::Selectable("Copy"))
                editor->setCopiedEntity(node);

            if (ImGui::Selectable("Cut"))
                editor->setCopiedEntity(node, true);

            auto copiedEntity = editor->getCopiedEntity();
            if (copiedEntity != entt::null && registry.valid(copiedEntity)) {
                if (ImGui::Selectable("Paste")) {
                    if (!registry.valid(copiedEntity)) {
                        editor->setCopiedEntity(entt::null);
                    } else {
                        scene->duplicateEntity(copiedEntity, node);

                        if (editor->getCutCopyEntity())
                            deleteEntity = true;
                    }
                }
            } else {
                ImGui::TextDisabled("Paste");
            }

            ImGui::Separator();

            if (ImGui::Selectable("Duplicate")) {
                scene->duplicateEntity(node);
            }
            if (ImGui::Selectable("Delete"))
                deleteEntity = true;
            if (editor->getSelected() == node)
                editor->setSelected(entt::null);
            ImGui::Separator();
            if (ImGui::Selectable("Rename"))
                doubleClicked = node;
            ImGui::Separator();

            if (ImGui::Selectable("Add Child")) {
                auto child = scene->createEntity();
                hierarchySystem->setParent(child, node);
            }
            ImGui::EndPopup();
        }

        if (!isDoubleClicked && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            auto ptr = node;
            ImGui::SetDragDropPayload("SCENE_HIERARCHY_ITEM", &ptr, sizeof(entt::entity*));
            ImGui::Text(ICON_MDI_ARROW_UP);
            ImGui::EndDragDropSource();
        }

        if (const ImGuiPayload* payload = ImGui::GetDragDropPayload(); payload && payload->IsDataType("SCENE_HIERARCHY_ITEM")) {
            bool acceptable;

            auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
            auto hierarchy = registry.try_get<HierarchyComponent>(entity);
            if (hierarchy) {
                acceptable = entity != node && (!hierarchySystem->isParent(entity, node)) && (hierarchy->parent != node);
            } else
                acceptable = entity != node;

            if (ImGui::BeginDragDropTarget()) {
                // Drop directly on to node and append to the end of it's children list.
                if (ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                    if (acceptable) {
                        if(hierarchy)
                            hierarchySystem->reparent(entity, node, *hierarchy);
                        else
                            registry.emplace<HierarchyComponent>(entity, node);
                        hadRecentDroppedEntity = node;
                    }
                }

                ImGui::EndDragDropTarget();
            }

            if (editor->getSelected() == entity)
                editor->setSelected(entt::null);
        }

        if (ImGui::IsItemClicked() && !deleteEntity)
            editor->setSelected(node);
        else if (doubleClicked == node && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered(ImGuiHoveredFlags_None))
            doubleClicked = entt::null;

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
            doubleClicked = node;
            if (editor->getState() == EditorState::Preview) {
                /*if (auto transform = registry.try_get<TransformComponent>(node))
                    editor->focusCamera(transform->position, 2.0f, 2.0f);*/
                // TODO: Implement
            }
        }

        if (deleteEntity) {
            hierarchySystem->destroyParent(node);
            if (nodeOpen)
                ImGui::TreePop();

            ImGui::PopID();
            return;
        }

        if (selectUp) {
            if (registry.valid(currentPrevious) && editor->getSelected() == node) {
                selectUp = false;
                editor->setSelected(currentPrevious);
            }
        }

        if (selectDown) {
            if (registry.valid(currentPrevious) && currentPrevious == editor->getSelected()) {
                selectDown = false;
                editor->setSelected(node);
            }
        }

        currentPrevious = node;

        if (nodeOpen == false) {
            ImGui::PopID();
            return;
        }

        const ImColor TreeLineColor = ImColor{128, 128, 128, 128};
        const float smallOffsetX = 6.0f;
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
        verticalLineStart.x += smallOffsetX; // to nicely line up with the arrow symbol
        ImVec2 verticalLineEnd = verticalLineStart;

        if (!noChildren) {
            auto child = hierarchy->first;
            while (child != entt::null && registry.valid(child)) {
                float horizontalTreeLineSize = 16.0f;
                auto currentPos = ImGui::GetCursorScreenPos();
                ImGui::Indent(10.0f);

                if (auto childHierarchy = registry.try_get<HierarchyComponent>(child)) {
                    auto firstChild = childHierarchy->first;
                    if (firstChild != entt::null && registry.valid(firstChild)) {
                        horizontalTreeLineSize *= 0.5f;
                    }
                }

                drawNode(child, registry);
                ImGui::Unindent(10.0f);

                const ImRect childRect = ImRect{ currentPos, currentPos + ImVec2{0.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y} };

                const float midpoint = (childRect.Min.y + childRect.Max.y) * 0.5f;
                drawList->AddLine(ImVec2{verticalLineStart.x, midpoint},
                                  ImVec2{verticalLineStart.x + horizontalTreeLineSize, midpoint}, TreeLineColor);
                verticalLineEnd.y = midpoint;

                if (registry.valid(child)) {
                    auto childHierarchy = registry.try_get<HierarchyComponent>(child);
                    child = childHierarchy ? childHierarchy->next : entt::null;
                }
            }
        }

        drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

        ImGui::TreePop();
        ImGui::PopID();
    }
}
