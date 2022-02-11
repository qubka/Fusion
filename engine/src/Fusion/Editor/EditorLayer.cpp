#include "EditorLayer.hpp"

using namespace Fusion;

EditorLayer::EditorLayer() : Layer("EditorLayer"), contentBrowserPanel{"assets"} {

}

EditorLayer::~EditorLayer() {

}


void EditorLayer::onAttach() {

    activeScene = std::make_shared<Scene>();

    editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

#if 0
    // Entity
		auto square = m_ActiveScene->CreateEntity("Green Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			virtual void OnCreate() override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;
			}

			virtual void OnDestroy() override
			{
			}

			virtual void OnUpdate(Timestep ts) override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;

				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::onDetach() {

}

void EditorLayer::onUpdate() {

}

void EditorLayer::onImGui() {
    if (activeScene)
        sceneHierarchyPanel.onImGui();

    contentBrowserPanel.onImGui();
}

void EditorLayer::openScene() {

}

void EditorLayer::newScene() {

}

void EditorLayer::saveSceneAs() {

}
