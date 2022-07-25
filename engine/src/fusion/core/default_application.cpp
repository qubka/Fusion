#include "default_application.hpp"
#include "engine.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"
#include "fusion/scene/scene_manager.hpp"

#include <cereal/archives/json.hpp>

#if FUSION_PLATFORM_WINDOWS
#include <windows.h>
#endif

using namespace fe;

DefaultApplication::DefaultApplication(std::string name) : Application{std::move(name)} {
#if FUSION_PLATFORM_MAC
    executablePath = fs::canonical("/proc/self/exe").parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
#else
    executablePath = fs::canonical("/proc/self/exe").parent_path().parent_path().parent_path();
#endif

    LOG_INFO << "Working directory: \"" << executablePath << "\"";
    fs::current_path(executablePath);

    projectSettings.projectVersion = getVersion().toString();
}

DefaultApplication::~DefaultApplication() {
    serialise();
}

void DefaultApplication::onStart() {
    LOG_INFO << "Default application starting!";

    auto vfs = VirtualFileSystem::Get();

    //FileSystem::Get()->addSearchPath(executablePath, executablePath.string());
    fs::path shaderPath{ executablePath / "engine" / "assets" / "shaders" };
    vfs->mount("EngineShaders", shaderPath);
    fs::path modelPath{ executablePath / "engine" / "assets" / "models" };
    vfs->mount("EngineModels", modelPath);

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
    fs::path iconPath{ executablePath / "engine" / "assets" / "icons" };
    vfs->mount("EngineIcons", iconPath);
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

    if (!fs::exists(projectSettings.projectRoot)) {
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
    if (!fs::exists(assetPath))
        fs::create_directory(assetPath);

    fs::path scriptPath{ assetPath / "scripts" };
    if (!fs::exists(scriptPath))
        fs::create_directory(scriptPath);

    fs::path shaderPath{ assetPath / "shaders" };
    if (!fs::exists(shaderPath))
        fs::create_directory(shaderPath);

    fs::path scenePath{ assetPath / "scenes" };
    if (!fs::exists(scenePath))
        fs::create_directory(scenePath);

    fs::path texturePath{ assetPath / "textures" };
    if (!fs::exists(texturePath))
        fs::create_directory(texturePath);

    fs::path meshPath{ assetPath / "meshes" };
    if (!fs::exists(meshPath))
        fs::create_directory(meshPath);

    fs::path soundPath{ assetPath / "sounds" };
    if (!fs::exists(soundPath))
        fs::create_directory(soundPath);

    mountPaths();

    SceneManager::Get()->setScene(std::make_unique<Scene>("Empty Scene"));

    serialise();

    projectLoaded = true;

    // Win32 : Sets up a console window and redirects standard output to it
    showConsole();
}

void DefaultApplication::openProject(const fs::path& path) {
    projectSettings.projectName = path.filename().replace_extension().string();
    projectSettings.projectRoot = path.parent_path();

    mountPaths();

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

    LOG_INFO << "Serialising application: \"" << projectPath << "\"";
}

void DefaultApplication::deserialise() {
    if (projectSettings.projectRoot.empty() || projectSettings.projectName.empty()) {
        LOG_INFO << "No saved Project file found";
        return;
    }

    fs::path projectPath{ projectSettings.projectRoot / (projectSettings.projectName + ".fsproj") };
    if (!fs::exists(projectPath)) {
        LOG_INFO << "No saved Project file found: \"" << projectPath << "\"";
        return;
    }

    mountPaths();

    try {
        std::istringstream is{FileSystem::ReadText(projectPath)};
        cereal::JSONInputArchive input{is};
        input(*this);
    }
    catch (...) {
        projectSettings = {};
        projectSettings.projectVersion = getVersion().toString();

        SceneManager::Get()->setScene(std::make_unique<Scene>("Empty Scene"));

        LOG_ERROR << "Failed to load project";
        return;
    }

    projectLoaded = true;

    LOG_INFO << "Deserialise application: \"" << projectPath << "\"";
}

void DefaultApplication::mountPaths() const {
    auto vfs = VirtualFileSystem::Get();
    fs::path assetPath{ projectSettings.projectRoot / "assets" };
    vfs->mount("Assets", assetPath);
    vfs->mount( "Meshes", assetPath / "meshes");
    vfs->mount("Textures", assetPath / "textures");
    vfs->mount("Sounds", assetPath / "sounds");
    vfs->mount("Scripts", assetPath / "scripts");
    vfs->mount("Shaders", assetPath / "shaders");
    vfs->mount("Scenes", assetPath / "scenes");
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
        SetConsoleTitle(TEXT(windowInfo.title.c_str()));
        consoleOpened = true;
    } else if (!projectSettings.isShowConsole && consoleOpened) {
        FreeConsole();
        consoleOpened = false;
    }
#else
    consoleOpened = true;
#endif
}