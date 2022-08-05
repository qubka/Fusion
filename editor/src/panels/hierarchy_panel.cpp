#include "hierarchy_panel.hpp"
#include "editor.hpp"

#include "fusion/input/input.hpp"
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
    auto input = Input::Get();
    selectUp = input->getKey(Key::Up);
    selectDown = input->getKey(Key::Down);

    ImGui::Begin(title.c_str(), &active, flags);
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

        if (ImGui::BeginPopup("AddEntity", ImGuiWindowFlags_AlwaysAutoResize)) {
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
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
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
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0.0f, style.FramePadding.y});
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
                        auto entity = *static_cast<entt::entity*>(payload->Data);
                        hierarchySystem->removeParent(entity);
                        LOG_INFO << "Unparent";
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Indent();

                //ImGuiUtils::AlternatingRowsBackground(ImGui::GetFontSize() + style.FramePadding.y);

                registry.each([&](auto entity) {
                    if (hierarchySystem->getParent(entity) == entt::null)
                        drawNode(entity, registry);
                });

                if (const ImGuiPayload* payload = ImGui::GetDragDropPayload(); payload && payload->IsDataType("SCENE_HIERARCHY_ITEM")) {
                    auto entity = *static_cast<entt::entity*>(payload->Data);
                    if (hierarchySystem->getParent(entity) != entt::null) {
                        ImVec2 minSpace{ ImGui::GetWindowContentRegionMin() };
                        ImVec2 maxSpace{ ImGui::GetWindowContentRegionMax() };

                        float yScroll = ImGui::GetScrollY();
                        float yOffset = glm::max(45.0f, yScroll); // Dont include search bar
                        minSpace.x += ImGui::GetWindowPos().x + 1.0f;
                        minSpace.y += ImGui::GetWindowPos().y + 1.0f + yOffset;
                        maxSpace.x += ImGui::GetWindowPos().x - 1.0f;
                        maxSpace.y += ImGui::GetWindowPos().y - 1.0f + yScroll;

                        if (ImGui::BeginDragDropTargetCustom(ImRect{ minSpace, maxSpace }, ImGui::GetID("Panel Hierarchy"))) {
                            if (const ImGuiPayload* customPayload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                                entity = *static_cast<entt::entity*>(customPayload->Data);
                                hierarchySystem->removeParent(entity);
                                LOG_INFO << "Unparent";
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

    const auto nameComponent = registry.try_get<NameComponent>(node);
    std::string name{ nameComponent ? *nameComponent : std::to_string(entt::to_integral(node)) };

    if (hierarchyFilter.IsActive()) {
        if (!hierarchyFilter.PassFilter(name.c_str())) {
            show = false;
        }
    }

    if (show) {
        auto scene = SceneManager::Get()->getScene();
        auto hierarchySystem = scene->getSystem<HierarchySystem>();

        ImGui::PushID(static_cast<int>(node));
        auto hierarchyComponent = registry.try_get<HierarchyComponent>(node);
        bool hasChildren = (hierarchyComponent != nullptr && hierarchyComponent->children > 0); // hierarchySystem->hasChildren(node)

        ImGuiTreeNodeFlags nodeFlags = ((editor->getSelected() == node) ? ImGuiTreeNodeFlags_Selected : 0);

        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding |
                ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

        if (!hasChildren) {
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
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{1.0f, 2.0f});

        if (hadRecentDroppedEntity == node) {
            ImGui::SetNextItemOpen(true);
            hadRecentDroppedEntity = entt::null;
        }

        std::string icon{ ICON_MDI_CUBE_OUTLINE };

        if (registry.any_of<CameraComponent>(node)) {
            editor->getComponentIcon(type_id<CameraComponent>, icon);
        } else if (registry.any_of<RigidbodyComponent>(node)) {
            editor->getComponentIcon(type_id<RigidbodyComponent>, icon);
        } else if (registry.any_of<LightComponent>(node)) {
            editor->getComponentIcon(type_id<LightComponent>, icon);
        }

        //ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorScheme::GetIconColor());
        // ImGui::BeginGroup();
        bool nodeOpen = ImGui::TreeNodeEx((void*) (intptr_t) entt::to_integral(node), nodeFlags, "%s", icon.c_str());
        {
            // Allow clicking of icon and text. Need twice as they are separated
            if (ImGui::IsItemClicked())
                editor->setSelected(node);
            else if (doubleClicked == node && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered(ImGuiHoveredFlags_None))
                doubleClicked = entt::null;
        }

        //ImGui::PopStyleColor();
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

            /*if (ImGui::Selectable("Undo"))

            if (ImGui::Selectable("Redo"))*/

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
                hierarchySystem->assignChild(node, child);
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

            auto entity = *static_cast<entt::entity*>(payload->Data);
            auto hierarchy = registry.try_get<HierarchyComponent>(entity);
            if (hierarchy) {
                acceptable = entity != node && (!hierarchySystem->isParent(entity, node)) && (hierarchy->parent != node);
            } else
                acceptable = entity != node;

            if (ImGui::BeginDragDropTarget()) {
                // Drop directly on to node and append to the end of it's children list.
                if (ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM")) {
                    if (acceptable) {
                        hierarchySystem->assignChild(node, entity);
                        hadRecentDroppedEntity = node;
                        LOG_INFO << "Parent";
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
                if (auto transform = registry.try_get<TransformComponent>(node))
                    editor->focusCamera(transform->getWorldPosition(), 5.0f, 0.2f);
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

        const ImU32 treeLineColor = IM_COL32(128, 128, 128, 128);
        const float smallOffsetX = 6.0f;
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 verticalLineStart{ ImGui::GetCursorScreenPos() };
        verticalLineStart.x += smallOffsetX; // to nicely line up with the arrow symbol
        ImVec2 verticalLineEnd = verticalLineStart;

        if (hasChildren) {
            auto child = hierarchyComponent->first;
            while (child != entt::null) {
                auto hierarchy = registry.try_get<HierarchyComponent>(child);

                float horizontalTreeLineSize = 16.0f;
                ImVec2 currentPos{ ImGui::GetCursorScreenPos() };
                ImGui::Indent(10.0f);

                if (hierarchy && hierarchy->children > 0) {
                    horizontalTreeLineSize *= 0.5f;
                }

                auto next = hierarchy ? hierarchy->next : entt::null;
                drawNode(child, registry);
                child = next;

                ImGui::Unindent(10.0f);

                ImRect childRect{currentPos, currentPos + ImVec2{0.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y}};

                const float midpoint = (childRect.Min.y + childRect.Max.y) * 0.5f;
                drawList->AddLine(ImVec2{verticalLineStart.x, midpoint},
                                  ImVec2{verticalLineStart.x + horizontalTreeLineSize, midpoint}, treeLineColor);
                verticalLineEnd.y = midpoint;
            }
        }

        drawList->AddLine(verticalLineStart, verticalLineEnd, treeLineColor);

        ImGui::TreePop();
        ImGui::PopID();
    }
}
