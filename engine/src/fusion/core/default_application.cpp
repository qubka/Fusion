#include "default_application.h"
#include "engine.h"

#include "fusion/devices/device_manager.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/assets/asset_registry.h"
#include "fusion/scripting/script_engine.h"

#if FUSION_PLATFORM_WINDOWS
#include <windows.h>
#elif FUSION_PLATFORM_ANDROID

#endif

using namespace fe;

DefaultApplication::DefaultApplication(std::string_view name) : Application{name} {
#if FUSION_PLATFORM_WINDOWS
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    executablePath = fs::path{path}.parent_path().parent_path();
#elif FUSION_PLATFORM_MAC
    executablePath = fs::canonical("/proc/self/exe").parent_path().parent_path().parent_path().parent_path().parent_path();
#elif FUSION_PLATFORM_ANDROID
    executablePath = "/";
#elif FUSION_PLATFORM_LINUX
    executablePath = fs::canonical("/proc/self/exe").parent_path().parent_path();
#endif

    FS_LOG_INFO("Working directory: '{}'", executablePath);
    fs::current_path(executablePath);

    projectSettings.projectVersion = getVersion().toString();
}

DefaultApplication::~DefaultApplication() {
    serialise();
    projectLoaded = false;
}

void DefaultApplication::onStart() {
    FS_LOG_INFO("Default application starting!");

    deserialise();

    WindowInfo windowInfo = {};
    windowInfo.size = projectSettings.size;
    windowInfo.title = projectSettings.title;
    windowInfo.isBorderless = projectSettings.isBorderless;
    windowInfo.isResizable = projectSettings.isResizable;
    windowInfo.isFloating = projectSettings.isFloating;
    windowInfo.isFullscreen = projectSettings.isFullscreen;
    windowInfo.isVSync = projectSettings.isVSync;

    // Initialise the window
    auto window = DeviceManager::Get()->createWindow(windowInfo);
    window->OnClose().connect<&Engine::requestClose>(Engine::Get());

    // Sets icons to window
    fs::path iconPath{ FUSION_ASSET_PATH "icons" };
    std::vector<fs::path> iconPaths {
        iconPath / "icon-16.png", iconPath / "icon-24.png", iconPath / "icon-32.png",
        iconPath / "icon-48.png", iconPath / "icon-64.png", iconPath / "icon-96.png",
        iconPath / "icon-128.png", iconPath / "icon-192.png", iconPath / "icon-256.png"
    };
    window->setIcons(iconPaths);

    // Win32 : Sets up a console window and redirects standard output to it
    showConsole();
}

void DefaultApplication::openNewProject(const fs::path& path, std::string_view name) {
    projectSettings.projectRoot = path / name;
    projectSettings.projectName = name;

    if (!FileSystem::IsExists(projectSettings.projectRoot)) {
        fs::create_directory(projectSettings.projectRoot);
    }

    // Set Default values
    projectSettings.projectVersion = getVersion().toString();
    projectSettings.title = name;
    if (auto window = DeviceManager::Get()->getWindow(0)) {
        window->setTitle(name);
        projectSettings.size = window->getSize();
        projectSettings.isBorderless = window->isBorderless();
        projectSettings.isResizable = window->isResizable();
        projectSettings.isFloating = window->isFloating();
        projectSettings.isFullscreen = window->isFullscreen();
        projectSettings.isVSync = window->isVSync();
    } else {
        projectSettings.size = { 1280, 720 };
        projectSettings.isBorderless = false;
        projectSettings.isResizable = true;
        projectSettings.isFloating = false;
        projectSettings.isFullscreen = false;
        projectSettings.isVSync = false;
    }
    projectSettings.isShowConsole = true;

    fs::path assetPath{ projectSettings.projectRoot / "assets" };
    if (!FileSystem::IsExists(assetPath))
        fs::create_directory(assetPath);

    fs::path scriptPath{ assetPath / "scripts" };
    if (!FileSystem::IsExists(scriptPath))
        fs::create_directory(scriptPath);

    fs::path shaderPath{ assetPath / "shaders" };
    if (!FileSystem::IsExists(shaderPath))
        fs::create_directory(shaderPath);

    fs::path scenePath{ assetPath / "scenes" };
    if (!FileSystem::IsExists(scenePath))
        fs::create_directory(scenePath);

    fs::path texturePath{ assetPath / "textures" };
    if (!FileSystem::IsExists(texturePath))
        fs::create_directory(texturePath);

    fs::path meshPath{ assetPath / "meshes" };
    if (!FileSystem::IsExists(meshPath))
        fs::create_directory(meshPath);

    fs::path soundPath{ assetPath / "sounds" };
    if (!FileSystem::IsExists(soundPath))
        fs::create_directory(soundPath);

    SceneManager::Get()->setScene(std::make_unique<Scene>("Empty Scene"));

    onProjectLoad();

    serialise();

    // Win32 : Sets up a console window and redirects standard output to it
    showConsole();
}

void DefaultApplication::openProject(const fs::path& path) {
    projectSettings.projectName = path.filename().replace_extension().string();
    projectSettings.projectRoot = path.parent_path();

    deserialise();
}

void DefaultApplication::serialise() {
    if (!projectLoaded)
        return;

    std::stringstream ss;
    {
        // output finishes flushing its contents when it goes out of scope
        cereal::JSONOutputArchive output{ss};
        output(*this);
    }

    fs::path projectPath{ projectSettings.projectRoot / (projectSettings.projectName + ".fsproj") };
    FileSystem::WriteText(projectPath, ss.str());

    FS_LOG_INFO("Serialising application: '{}'", projectPath);
}

void DefaultApplication::deserialise() {
    if (projectSettings.projectRoot.empty() && projectSettings.projectName.empty()) {
        FS_LOG_INFO("No saved Project file found");
        return;
    }

    fs::path projectPath{ projectSettings.projectRoot / (projectSettings.projectName + ".fsproj") };
    if (!FileSystem::IsExists(projectPath)) {
        FS_LOG_INFO("No saved Project file found: '{}'", projectPath);
        return;
    }

    try {
        std::istringstream is{FileSystem::ReadText(projectPath)};
        cereal::JSONInputArchive input{is};
        input(*this);
    }
    catch (...) {
        projectSettings = {};
        projectSettings.projectVersion = getVersion().toString();

        SceneManager::Get()->setScene(std::make_unique<Scene>("Empty Scene"));

        FS_LOG_ERROR("Failed to load project");
        return;
    }

    onProjectLoad();

    FS_LOG_INFO("Deserialise application: '{}'", projectPath);
}

void DefaultApplication::showConsole() {
#if FUSION_PLATFORM_WINDOWS
    // Create console if needed
    if (projectSettings.isShowConsole && !consoleOpened) {
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        FILE* stream;
        freopen_s(&stream, "CONIN$", "r", stdin);
        freopen_s(&stream, "CONOUT$", "w+", stdout);
        freopen_s(&stream, "CONOUT$", "w+", stderr);
        SetConsoleTitle(TEXT(projectSettings.title.c_str()));
        consoleOpened = true;
    } else if (!projectSettings.isShowConsole && consoleOpened) {
        FreeConsole();
        consoleOpened = false;
    }
#else
    consoleOpened = true;
#endif
}

void DefaultApplication::onProjectLoad() {
#if FUSION_SCRIPTING
    if (!projectSettings.scriptModulePath.empty()) {
        auto scriptEngine = ScriptEngine::Get();
        scriptEngine->setAssemblyPaths(FUSION_ASSET_PATH "scripts/Fusion-ScriptCore.dll", projectSettings.projectRoot / projectSettings.scriptModulePath);
        scriptEngine->reloadAssembly();
    }
#endif
    AssetRegistry::Get()->releaseAll();

    projectLoaded = true;
}
