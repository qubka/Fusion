#include "inspector_panel.h"
#include "editor.h"

#include "fusion/scene/components.h"
#include "fusion/models/mesh.h"
#include "fusion/scripting/script_engine.h"

using namespace fe;

namespace ImGui {
    template<>
    void ComponentEditorWidget<TransformComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& transform = registry.get<TransformComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        glm::vec3 position{ transform.getLocalPosition() };
        if (ImGuiUtils::PropertyControl("Position", position, 0.0f, 0.0f, 0.0f, 0.01f)) {
            transform.setLocalPosition(position);
        }

        glm::vec3 rotation{ glm::degrees(glm::eulerAngles(transform.getLocalOrientation())) };
        if (ImGuiUtils::PropertyControl("Rotation", rotation)) {
            transform.setLocalOrientation(glm::radians(rotation));
        }

        glm::vec3 scale{ transform.getLocalScale() };
        if (ImGuiUtils::PropertyControl("Scale", scale, 0.01f, FLT_MAX, 1.0f, 0.01f)) {
            transform.setLocalScale(scale);
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<CameraComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& camera = registry.get<CameraComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        bool ortho = camera.isOrthographic();
        if (ImGuiUtils::Property("Orthographic", ortho)) {
            camera.setOrthographic(ortho);
        }

        if (!ortho) {
            float fov = camera.getFov();
            if (ImGuiUtils::Property("Fov", fov, 1.0f, 120.0f)) {
                camera.setFov(fov);
            }
        }

        float nearClip = camera.getNearClip();
        if (ImGuiUtils::Property("Near Clip", nearClip, 0.0f, 10.0f)) {
            camera.setNearClip(nearClip);
        }

        float farClip = camera.getFarClip();
        if (ImGuiUtils::Property("Far Clip", farClip, 10.0f, 10000.0f)) {
            camera.setFarClip(farClip);
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

#if FUSION_SCRIPTING
    namespace Widget {
        template<typename T>
        inline void SetFieldProperty(ScriptInstance& scriptInstance, const std::string& name) {
            T data = scriptInstance.getFieldValue<T>(name);
            if (ImGuiUtils::Property(name.c_str(), data)) {
                scriptInstance.setFieldValue(name, data);
            }
        }

        template<typename T>
        inline void SetFieldProperty(ScriptFieldInstance& scriptField, const std::string& name) {
            T data = scriptField.getValue<T>();
            if (ImGuiUtils::Property(name.c_str(), data)) {
                scriptField.setValue(data);
            }
        }

        template<typename T>
        inline void SetFieldProperty(ScriptFieldMap& entityFields, const ScriptField& scriptField, const std::string& name, T data = {}) {
            if (ImGuiUtils::Property(name.c_str(), data)) {
                ScriptFieldInstance& fieldInstance = entityFields[name];
                fieldInstance.field = scriptField;
                fieldInstance.setValue(data);
            }
        }

        template<typename T>
        inline void SetFieldPropertyControl(ScriptInstance& scriptInstance, const std::string& name) {
            T data = scriptInstance.getFieldValue<T>(name);
            if (ImGuiUtils::PropertyControl(name.c_str(), data)) {
                scriptInstance.setFieldValue(name, data);
            }
        }

        template<typename T>
        inline void SetFieldPropertyControl(ScriptFieldInstance& scriptField, const std::string& name) {
            T data = scriptField.getValue<T>();
            if (ImGuiUtils::PropertyControl(name.c_str(), data)) {
                scriptField.setValue(data);
            }
        }

        template<typename T>
        inline void SetFieldPropertyControl(ScriptFieldMap& entityFields, const ScriptField& scriptField, const std::string& name, T data = {}) {
            if (ImGuiUtils::PropertyControl(name.c_str(), data)) {
                ScriptFieldInstance& fieldInstance = entityFields[name];
                fieldInstance.field = scriptField;
                fieldInstance.setValue(data);
            }
        }
    }

    template<>
    void ComponentEditorWidget<ScriptComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& script = registry.get<ScriptComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        auto scriptEngine = ScriptEngine::Get();

        auto& entityClasses = scriptEngine->getEntityClasses();

        int32_t selected = -1;
        std::vector<const char*> options;
        options.reserve(entityClasses.size());
        for (const auto& [i, entry] : enumerate(entityClasses)) {
            const auto& key = entry.first;
            options.push_back(key.c_str());
            if (key == script.className)
                selected = i;
        }

        if (ImGuiUtils::PropertyDropdown("Class", options, selected)) {
            script.className = options[selected];
        }

        // Fields
        auto scene = scriptEngine->getSceneContext();
        bool sceneRunning = scene && scene->isRuntime();
        if (sceneRunning) {
            auto& scriptInstance = script.instance;
            if (scriptInstance) {
                const auto& scriptFields = scriptInstance->getScriptClass()->getFields();
                for (const auto& [name, field] : scriptFields) {
                    switch (field.type) {
                        case ScriptFieldType::Float:
                            Widget::SetFieldProperty<float>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Double:
                            Widget::SetFieldProperty<double>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Bool:
                            Widget::SetFieldProperty<bool>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Char:
                            Widget::SetFieldProperty<char>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Byte:
                            Widget::SetFieldProperty<int8_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Short:
                            Widget::SetFieldProperty<int16_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Int:
                            Widget::SetFieldProperty<int32_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Long:
                            Widget::SetFieldProperty<int64_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::UByte:
                            Widget::SetFieldProperty<uint8_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::UShort:
                            Widget::SetFieldProperty<uint16_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::UInt:
                            Widget::SetFieldProperty<uint32_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::ULong:
                            Widget::SetFieldProperty<uint64_t>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Vector2:
                            Widget::SetFieldPropertyControl<glm::vec2>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Vector3:
                            Widget::SetFieldPropertyControl<glm::vec3>(*scriptInstance, name);
                            break;
                        case ScriptFieldType::Vector4:
                        case ScriptFieldType::Quaternion:
                            Widget::SetFieldPropertyControl<glm::vec4>(*scriptInstance, name);
                            break;
                        default:
                            break;
                    }
                }
            }
        } else {
            std::shared_ptr<ScriptClass> entityClass = scriptEngine->getEntityClass(script.className);
            if (entityClass) {
                const auto& scriptFields = entityClass->getFields();
                auto& entityFields = script.fields;
                for (const auto& [name, field] : scriptFields) {
                    // Field has been set in editor
                    if (auto it = entityFields.find(name); it != entityFields.end()) {
                        ScriptFieldInstance& scriptField = it->second;

                        if (scriptField.field.classField == nullptr)
                            scriptField.field = field;

                        // Display control to set it maybe
                        switch (field.type) {
                            case ScriptFieldType::Float:
                                Widget::SetFieldProperty<float>(scriptField, name);
                                break;
                            case ScriptFieldType::Double:
                                Widget::SetFieldProperty<double>(scriptField, name);
                                break;
                            case ScriptFieldType::Bool:
                                Widget::SetFieldProperty<bool>(scriptField, name);
                                break;
                            case ScriptFieldType::Char:
                                Widget::SetFieldProperty<char>(scriptField, name);
                                break;
                            case ScriptFieldType::Byte:
                                Widget::SetFieldProperty<int8_t>(scriptField, name);
                                break;
                            case ScriptFieldType::Short:
                                Widget::SetFieldProperty<int16_t>(scriptField, name);
                                break;
                            case ScriptFieldType::Int:
                                Widget::SetFieldProperty<int32_t>(scriptField, name);
                                break;
                            case ScriptFieldType::Long:
                                Widget::SetFieldProperty<int64_t>(scriptField, name);
                                break;
                            case ScriptFieldType::UByte:
                                Widget::SetFieldProperty<uint8_t>(scriptField, name);
                                break;
                            case ScriptFieldType::UShort:
                                Widget::SetFieldProperty<uint16_t>(scriptField, name);
                                break;
                            case ScriptFieldType::UInt:
                                Widget::SetFieldProperty<uint32_t>(scriptField, name);
                                break;
                            case ScriptFieldType::ULong:
                                Widget::SetFieldProperty<uint64_t>(scriptField, name);
                                break;
                            case ScriptFieldType::Vector2:
                                Widget::SetFieldPropertyControl<glm::vec2>(scriptField, name);
                                break;
                            case ScriptFieldType::Vector3:
                                Widget::SetFieldPropertyControl<glm::vec3>(scriptField, name);
                                break;
                            case ScriptFieldType::Vector4:
                            case ScriptFieldType::Quaternion:
                                Widget::SetFieldPropertyControl<glm::vec4>(scriptField, name);
                                break;
                            default:
                                break;
                        }
                    } else {
                        // Display control to set it maybe
                        switch (field.type) {
                            case ScriptFieldType::Float:
                                Widget::SetFieldProperty<float>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Double:
                                Widget::SetFieldProperty<double>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Bool:
                                Widget::SetFieldProperty<bool>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Char:
                                Widget::SetFieldProperty<char>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Byte:
                                Widget::SetFieldProperty<int8_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Short:
                                Widget::SetFieldProperty<int16_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Int:
                                Widget::SetFieldProperty<int32_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Long:
                                Widget::SetFieldProperty<int64_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::UByte:
                                Widget::SetFieldProperty<uint8_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::UShort:
                                Widget::SetFieldProperty<uint16_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::UInt:
                                Widget::SetFieldProperty<uint32_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::ULong:
                                Widget::SetFieldProperty<uint64_t>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Vector2:
                                Widget::SetFieldPropertyControl<glm::vec2>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Vector3:
                                Widget::SetFieldPropertyControl<glm::vec3>(entityFields, field, name);
                                break;
                            case ScriptFieldType::Vector4:
                            case ScriptFieldType::Quaternion:
                                Widget::SetFieldPropertyControl<glm::vec4>(entityFields, field, name);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }
#endif

    template<>
    void ComponentEditorWidget<MeshComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& mesh = registry.get<MeshComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        /*static ImGuiTextFilter filter;
        static std::shared_ptr<Asset> selected;
        std::shared_ptr<Asset> current = mesh;
        if (ImGuiUtils::PropertyAsset<Mesh>("Mesh Filter", current, selected, filter)) {
            if (current) {
                mesh = std::dynamic_pointer_cast<Mesh>(current);
            } else {
                mesh.reset();
            }
        }*/

        // Readonly
        if (auto filter = mesh.get()) {
            ImGui::NewLine();
            ImGui::Separator();
            if (mesh.model)
                ImGuiUtils::PropertyText("Path", mesh.model->getPath().string().c_str());
            if (filter->getIndex() != UINT32_MAX)
                ImGuiUtils::Property("Index", filter->getIndex());
            ImGuiUtils::Property("Vertex Count", filter->getVertexCount());
            ImGuiUtils::Property("Index Count", filter->getIndexCount());
            ImGuiUtils::Property("Min", filter->getBoundingBox().getMin());
            ImGuiUtils::Property("Max", filter->getBoundingBox().getMax());
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<TextComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& text = registry.get<TextComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        static ImGuiTextFilter filter;
        static std::shared_ptr<Asset> selected;

        ImGuiUtils::PropertyTextbox("Text", text.text);
        {
            std::shared_ptr<Asset> current = text.font;
            if (ImGuiUtils::PropertyAsset<Font>("Font", current, selected, filter)) {
                if (current) {
                    text.font = std::dynamic_pointer_cast<Font>(current);
                } else {
                    text.font.reset();
                }
            }
        }
        ImGuiUtils::Property("Color", text.color, 0.0f, 0.0f, 1.0f, ImGuiUtils::PropertyType::Color);
        ImGuiUtils::Property("Kerning", text.kerning, 0.0f, 0.0f, 0.001f);
        ImGuiUtils::Property("Line Spacing", text.lineSpacing, 0.0f, 0.0f, 0.001f);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<LightComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& light = registry.get<LightComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::PropertyDropdown("Type", light.type);
        ImGuiUtils::Property("Color", light.color, 0.0f, 0.0f, 1.0f, ImGuiUtils::PropertyType::Color);
        ImGuiUtils::Property("Radius", light.radius, 0.01f, 5.0f, 0.005f);
        ImGuiUtils::Property("Ambient", light.ambient, 0.0f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
        ImGuiUtils::Property("Diffuse", light.diffuse, 0.0f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
        ImGuiUtils::Property("Specular", light.specular, 0.0f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
        if (light.type != LightComponent::LightType::Directional) {
            ImGuiUtils::Property("Constant", light.constant, 0.01f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
            ImGuiUtils::Property("Linear", light.linear, 0.01f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
            ImGuiUtils::Property("Quadratic", light.quadratic, 0.01f, 5.0f, 0.01f, ImGuiUtils::PropertyType::Slider);
        }
        if (light.type == LightComponent::LightType::Spot) {
            ImGuiUtils::Property("Cut Off", light.cutOff, 0.01f, 360.f, 0.01f, ImGuiUtils::PropertyType::Slider);
            ImGuiUtils::Property("Outer CutOff", light.outerCutOff, 0.01f, 360.f, 0.01f, ImGuiUtils::PropertyType::Slider);
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<MaterialComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& material = registry.get<MaterialComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        static ImGuiTextFilter filter;
        static std::shared_ptr<Asset> selected;

        ImGuiUtils::Property("Base Color", material.baseColor, 0.0f, 1.0f, 0.01f, ImGuiUtils::PropertyType::Color);

        {
            std::shared_ptr<Asset> current = material.diffuse;
            if (ImGuiUtils::PropertyAsset<Texture2d>("Diffuse", current, selected, filter)) {
                if (current) {
                    material.diffuse = std::dynamic_pointer_cast<Texture2d>(current);
                } else {
                    material.diffuse.reset();
                }
            }
        }
        {
            std::shared_ptr<Asset> current = material.specular;
            if (ImGuiUtils::PropertyAsset<Texture2d>("Specular", current, selected, filter)) {
                if (current) {
                    material.specular = std::dynamic_pointer_cast<Texture2d>(current);
                } else {
                    material.specular.reset();
                }
            }
        }
        {
            std::shared_ptr<Asset> current = material.normal;
            if (ImGuiUtils::PropertyAsset<Texture2d>("Normal", current, selected, filter)) {
                if (current) {
                    material.normal = std::dynamic_pointer_cast<Texture2d>(current);
                } else {
                    material.normal.reset();
                }
            }
        }

        ImGuiUtils::Property("Shininess", material.shininess, 0.0f, 128.0f, 0.01f, ImGuiUtils::PropertyType::Slider);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<RigidbodyComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& rigidbody = registry.get<RigidbodyComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::PropertyDropdown("Type", rigidbody.type);
        ImGuiUtils::Property("Mass", rigidbody.mass, 0.0f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Linear Drag", rigidbody.linearDrag, 0.0f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Angular Drag", rigidbody.angularDrag, 0.0f, FLT_MAX, 0.01f);
        ImGuiUtils::PropertyControl("Freeze Position", rigidbody.freezePosition);
        ImGuiUtils::PropertyControl("Freeze Rotation", rigidbody.freezeRotation);
        ImGuiUtils::Property("Disable Gravity", rigidbody.disableGravity);
        ImGuiUtils::Property("Is Kinematic", rigidbody.kinematic);
        ImGuiUtils::Property("Is Sleep", rigidbody.sleep);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<BoxColliderComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& collider = registry.get<BoxColliderComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::PropertyControl("Center", collider.center, 0.0f, 0.0f, 0.0f, 0.01f);
        ImGuiUtils::PropertyControl("Extent", collider.extent, 0.01f, FLT_MAX, 0.01f, 0.01f);
        ImGuiUtils::Property("Is Trigger", collider.trigger);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<SphereColliderComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& collider = registry.get<SphereColliderComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::PropertyControl("Center", collider.center, 0.0f, 0.0f, 0.0f, 0.01f);
        ImGuiUtils::Property("Radius", collider.radius, 0.01f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Is Trigger", collider.trigger);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<CapsuleColliderComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& collider = registry.get<CapsuleColliderComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::PropertyControl("Center", collider.center, 0.0f, 0.0f, 0.0f, 0.01f);
        ImGuiUtils::Property("Radius", collider.radius, 0.01f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Height", collider.height, 0.01f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Is Trigger", collider.trigger);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<MeshColliderComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& collider = registry.get<MeshColliderComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::Property("Is Convex", collider.convex);
        ImGuiUtils::Property("Is Trigger", collider.trigger);
        ImGuiUtils::Property("Override Mesh", collider.overrideMesh);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<PhysicsMaterialComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& material = registry.get<PhysicsMaterialComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        ImGuiUtils::Property("Dynamic Friction", material.dynamicFriction, 0.0f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Static Friction", material.staticFriction, 0.0f, FLT_MAX, 0.01f);
        ImGuiUtils::Property("Restitution", material.restitution, 0.0f, 1.0f, 0.01f);
        ImGuiUtils::PropertyDropdown("Friction Combine", material.frictionCombine);
        ImGuiUtils::PropertyDropdown("Restitution Combine", material.restitutionCombine);

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }

    template<>
    void ComponentEditorWidget<SkyboxComponent>(entt::registry& registry, entt::registry::entity_type entity) {
        auto& skybox = registry.get<SkyboxComponent>(entity);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3.0f);
        ImGui::Separator();

        static ImGuiTextFilter filter;
        static std::shared_ptr<Asset> selected;

        std::shared_ptr<Asset> current = skybox.texture;
        if (ImGuiUtils::PropertyAsset<TextureCube>("Texture", current, selected, filter)) {
            if (current) {
                skybox.texture = std::dynamic_pointer_cast<TextureCube>(current);
            } else {
                skybox.texture.reset();
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
    }
}

#define REG_COMPONENT(ComponentType, ComponentName)            \
{                                                              \
    std::string name;                                          \
    editor.getComponentIcon(type_id<ComponentType>, name);     \
    enttEditor.registerComponent<ComponentType>(name + ("\t" ComponentName)); \
}

InspectorPanel::InspectorPanel(Editor& editor) : EditorPanel{ICON_MDI_INFORMATION " Inspector###inspector", "Inspector", editor} {
    REG_COMPONENT(TransformComponent, "Transform");
    REG_COMPONENT(MeshComponent, "Mesh");
    REG_COMPONENT(CameraComponent, "Camera");
    REG_COMPONENT(MaterialComponent, "Material");
    REG_COMPONENT(RigidbodyComponent, "Rigidbody");
    REG_COMPONENT(BoxColliderComponent, "Box Collider");
    REG_COMPONENT(SphereColliderComponent, "Sphere Collider");
    REG_COMPONENT(CapsuleColliderComponent, "Capsule Collider");
    REG_COMPONENT(MeshColliderComponent, "Mesh Collider");
    REG_COMPONENT(PhysicsMaterialComponent, "Physics Material");
    REG_COMPONENT(LightComponent, "Light");
    REG_COMPONENT(TextComponent, "Text");
    REG_COMPONENT(SkyboxComponent, "Skybox");
#if FUSION_SCRIPTING
    REG_COMPONENT(ScriptComponent, "Script");
#endif
}

InspectorPanel::~InspectorPanel() {

}

void InspectorPanel::onImGui() {
    auto selected = editor.getSelected();

    if (ImGui::Begin(title.c_str(), &active)) {
        auto scene = SceneManager::Get()->getScene();
        if (!scene) {
            editor.setSelected(entt::null);
            ImGui::End();
            return;
        }

        auto& registry = scene->getRegistry();
        if (selected == entt::null || !registry.valid(selected)) {
            editor.setSelected(entt::null);
            ImGui::End();
            return;
        }

        // active checkbox
        auto activeComponent = registry.try_get<ActiveComponent>(selected);
        bool active = activeComponent ? activeComponent->active : true;
        if (ImGui::Checkbox("##ActiveCheckbox", &active)) {
            if (!activeComponent)
                registry.emplace<ActiveComponent>(selected, active);
            else
                activeComponent->active = active;
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(ICON_MDI_CUBE);
        ImGui::SameLine();

        auto nameComponent = registry.try_get<NameComponent>(selected);
        std::string name{ nameComponent ? *nameComponent : std::to_string(entt::to_integral(selected)) };

        if (debugMode) {
            ImGui::Text("ID: %d", entt::to_integral(selected));
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 2.0f);
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            if (ImGuiUtils::InputText("##InsEntityName", name))
                registry.get_or_emplace<NameComponent>(selected).name = std::move(name);
            ImGui::PopFont();
        }
        ImGui::SameLine();

        ImVec4 color{0.7f, 0.7f, 0.7f, 0.0f};
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorScheme::Hovered(color));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorScheme::Active(color));

        if (ImGui::Button(ICON_MDI_TUNE))
            ImGui::OpenPopup("SetDebugMode");

        ImGui::PopStyleColor(3);

        if (ImGui::BeginPopup("SetDebugMode", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
            if (ImGui::Selectable("Debug Mode", debugMode)) {
                debugMode = !debugMode;
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        if (debugMode) {
            ImGui::Text("ID: %d", entt::to_integral(selected));

            if (auto hierarchyComponent = registry.try_get<HierarchyComponent>(selected)) {
                auto parent = hierarchyComponent->parent;
                if (parent != entt::null) {
                    ImGui::Text("Parent : ID: %d", entt::to_integral(parent));
                    if (auto nameComponent = registry.try_get<NameComponent>(parent)) {
                        ImGui::SameLine();
                        ImGui::Text(" (%s)", nameComponent->name.c_str());
                    }
                } else {
                    ImGui::TextUnformatted("Parent : null");
                }

                ImGui::Text("Children : %d", hierarchyComponent->children);

                ImGui::Indent(24.0f);

                auto child = hierarchyComponent->first;
                while (child != entt::null) {
                    ImGui::Text("ID: %d", entt::to_integral(child));
                    if (auto nameComponent = registry.try_get<NameComponent>(child)) {
                        ImGui::SameLine();
                        ImGui::Text(" (%s)", nameComponent->name.c_str());
                    }
                    child = registry.get<HierarchyComponent>(child).next;
                }

                ImGui::Unindent(24.0f);
            }

            ImGui::Separator();
        }

        ImGui::BeginChild("Components", ImVec2{0.0f, 0.0f}, false, ImGuiWindowFlags_None);
        enttEditor.Render(registry, selected);
        ImGui::EndChild();
    }
    ImGui::End();
}
