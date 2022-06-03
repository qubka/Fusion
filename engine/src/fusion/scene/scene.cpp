#include "scene.hpp"
#include "components.hpp"

#include "fusion/renderer/systems/model_renderer.hpp"
#include "fusion/renderer/systems/grid_renderer.hpp"
#include "fusion/renderer/editor_camera.hpp"
#include "fusion/input/input.hpp"

using namespace fe;

Scene::Scene(std::string name) : name{std::move(name)} {
    world.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyComponent>>();
    //world.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyComponent>>();
    //world.on_update<TransformComponent>().connect<&TransformComponent::update>();
}

Scene::~Scene() {

}

void Scene::onViewportResize(const glm::vec2& size) {

}

void Scene::onUpdateRuntime(float dt) {

}

void Scene::onUpdateEditor(float dt) {

}

void Scene::onRenderRuntime() {

}

void Scene::onRenderEditor(const EditorCamera& camera) {

    /*if (Input::GetKey(Key::U)) {
        auto group = world.group<DirtyComponent, TransformComponent, RelationshipComponent>();
        std::cout << "___"<< std::endl;
        group.each([&](auto entity, auto transform, auto relation){
            std::cout << world.get<TagComponent>(entity).tag << std::endl;
        });

        group.sort([&](const entt::entity lhs, const entt::entity rhs) {
            const auto &left = world.get<RelationshipComponent>(lhs);
            const auto &right = world.get<RelationshipComponent>(rhs);
            return !(left.parent != entt::null && left.children < right.children);

        });
        //group.each([](auto &&...) {  });
        std::cout << "___"<< std::endl;
        group.each([&](auto entity, auto transform, auto relation){
            std::cout << world.get<TagComponent>(entity).tag << std::endl;
        });
    }*/

    auto group = world.group<DirtyComponent, RelationshipComponent, TransformComponent>();
    group.sort([&](const entt::entity lhs, const entt::entity rhs) {
        const auto& clhs = world.get<RelationshipComponent>(lhs);
        const auto& crhs = world.get<RelationshipComponent>(rhs);
        return !(clhs.parent != entt::null && clhs.children < crhs.children);
    });
    group.each([&](const auto entity, auto& relationship, auto& transform) {
        glm::mat4 m{ transform.transform() };
        if (relationship.parent != entt::null) {
            m *= world.get<TransformComponent>(relationship.parent).transform();
        }
        transform.model = m;
    });

    world.clear<DirtyComponent>();

    auto& modelRenderer = ModelRenderer::Instance();

    modelRenderer.begin();

    world.view<const TransformComponent>().each([&](const auto& transform) {
        modelRenderer.draw(transform.model);
    });

    modelRenderer.end();

    /*
     * const auto& [transform, relationship] = group.get<TransformComponent, RelationshipComponent>(entity);

        glm::mat4 m{ transform.transform() };
        if (relationship.parent != entt::null) {
            m *= world.get<TransformComponent>(relationship.parent).transform();
        }
        modelRenderer.draw(m);
     */

    //world.clear<DirtyComponent>();

    /*auto& modelRenderer = ModelRenderer::Instance();

    modelRenderer.begin();

    auto group = world.group<TransformComponent>(entt::get<ModelComponent>);
    for (const auto entity : group) {
        const auto& [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
        modelRenderer.draw();
    }

    modelRenderer.end();*/

    //world.sort<TransformComponent>([](const auto& lhs, const auto& rhs) { return lhs.dirty != rhs.dirty; });

    /*for (auto [entity, transform, localTransform] : world.view<TransformComponent, LocalTransformComponent>().each()) {
    }

    world.view<DirtyComponent>().each([&](const auto entity) {
        const auto& transform = world.get<TransformComponent>(entity);
    });

    auto group = world.group<DirtyComponent, TransformComponent>();
    group.sort([&](const entt::entity lhs, const entt::entity rhs) {
        return world.get<DirtyComponent>(lhs) != world.get<DirtyComponent>(rhs);
    });

    for (const auto entity : group) {
        const auto& [dirty, transform] = group.get<DirtyComponent, TransformComponent>(entity);



        dirty.flag = false;
    }*/

    /*auto& modelRenderer = ModelRenderer::Instance();

    modelRenderer.begin();

    auto group = world.group<TransformComponent>(entt::get<ModelComponent>);
    for (const auto entity : group) {
        const auto& [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
        modelRenderer.draw(transform.invTransform());
    }

    modelRenderer.end();*/

    auto& gridRenderer = GridRenderer::Instance();

    gridRenderer.begin();
    gridRenderer.draw();
    gridRenderer.end();
}