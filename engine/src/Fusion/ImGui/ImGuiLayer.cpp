#include "ImGuiLayer.hpp"
#include "Fusion.hpp"

#include <imguizmo/ImGuizmo.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

using namespace Fusion;

static void ImGuiErrorCallback(VkResult err) {
    if (err == 0)
        return;
    FE_LOG_ERROR << "[Imgui] Error: VkResult = " << err;
    FE_ASSERT(err >= 0 && "[Imgui] Fatal: Vulkan result!");
}

ImGuiLayer::ImGuiLayer(Renderer& renderer) : Layer{"ImGuiLayer"}, renderer{renderer} {

}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::onAttach() {
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    vk::DescriptorPoolSize poolSizes[] = {
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
           { vk::DescriptorType::eInputAttachment, 1000 }
    };

    vk::DescriptorPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

    // Create Descriptor Pool
    auto result = renderer.getVulkan().getDevice().createDescriptorPool(&poolInfo, nullptr, &imguiPool);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create descriptor imgui pool!");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    setDarkThemeColors();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(renderer.getVulkan().getWindow(), true);
    auto& vulkan = renderer.getVulkan();
    auto& swapChain = renderer.getSwapChain();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = vulkan.getInstance();
    initInfo.PhysicalDevice = vulkan.getPhysical();
    initInfo.Device = vulkan.getDevice();
    //initInfo.QueueFamily = g_QueueFamily;
    initInfo.Queue = vulkan.getGraphicsQueue();
    //initInfo.PipelineCache = g_PipelineCache;
    initInfo.DescriptorPool = imguiPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    initInfo.ImageCount = swapChain->imageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.CheckVkResultFn = ImGuiErrorCallback;
    ImGui_ImplVulkan_Init(&initInfo, swapChain->getRenderPass());

    // Load a first font
    io.Fonts->AddFontDefault();

    // Add character ranges and merge into the previous font
    // The ranges array is not copied by the AddFont* functions and is used lazily
    // so ensure it is available at the time of building or calling GetTexDataAsRGBA32().
    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig config;
    config.MergeMode = true;

    io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 18.0f, &config, icons_ranges); // Merge into first font
    io.Fonts->Build();

    //execute a gpu command to upload imgui font textures
    renderer.getVulkan().submit([&](vk::CommandBuffer& cmd) {
        ImGui_ImplVulkan_CreateFontsTexture(cmd);
    });

    //clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiLayer::onDetach() {
    // Cleanup
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    renderer.getVulkan().getDevice().destroyDescriptorPool(imguiPool, nullptr);
}

void ImGuiLayer::begin() {
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();
}

void ImGuiLayer::end(vk::CommandBuffer& commandBuffer) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void ImGuiLayer::setDarkThemeColors() {
    /*auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = { 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = { 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = { 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = { 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = { 0.15f, 0.1505f, 0.151f, 1.0f };*/
}