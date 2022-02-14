#pragma once

#include "Fusion/Core/Application.hpp"
#include "Fusion/Core/Log.hpp"
#include "Fusion/Core/Layer.hpp"
#include "Fusion/Core/Time.hpp"

#include "Fusion/Input/Input.hpp"
#include "Fusion/Input/BaseInput.hpp"
#include "Fusion/Input/KeyInput.hpp"
#include "Fusion/Input/KeyCodes.hpp"
#include "Fusion/Input/MouseInput.hpp"
#include "Fusion/Input/MouseCodes.hpp"

#include "Fusion/Renderer/Camera.hpp"
#include "Fusion/Renderer/PerspectiveCamera.hpp"
#include "Fusion/Renderer/OrthographicCamera.hpp"
#include "Fusion/Renderer/EditorCamera.hpp"
#include "Fusion/Renderer/Window.hpp"
#include "Fusion/Renderer/Vulkan.hpp"
#include "Fusion/Renderer/VulkanTools.hpp"
#include "Fusion/Renderer/Texture.hpp"
#include "Fusion/Renderer/SwapChain.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Renderer/Pipeline.hpp"
#include "Fusion/Renderer/Mesh.hpp"
#include "Fusion/Renderer/Image.hpp"
#include "Fusion/Renderer/Descriptors.hpp"
#include "Fusion/Renderer/AllocatedBuffer.hpp"

#include "Fusion/Geometry/AABB.hpp"
#include "Fusion/Geometry/Frustum.hpp"
#include "Fusion/Geometry/Plane.hpp"
#include "Fusion/Geometry/Ray.hpp"
#include "Fusion/Geometry/Sphere.hpp"
#include "Fusion/Geometry/Geometry.hpp"

#include "Fusion/Events/Event.hpp"
#include "Fusion/Events/WindowEvents.hpp"
#include "Fusion/Events/MouseEvents.hpp"
#include "Fusion/Events/KeyEvents.hpp"

#include "Fusion/Scene/Components.hpp"
#include "Fusion/Scene/Scene.hpp"
#include "Fusion/Scene/SceneCamera.hpp"
#include "Fusion/Scene/SceneSerializer.hpp"

#include "Fusion/Systems/RendererSystemBase.hpp"
#include "Fusion/Systems/MeshRenderer.hpp"

#include "Fusion/ImGui/ImGuiLayer.hpp"

#include "Fusion/Utils/ProcessInfo.hpp"
#include "Fusion/Utils/Extentions.hpp"

//#include "Fusion/EntryPoint.hpp" add for dynamic linking