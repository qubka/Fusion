#include "default_application.hpp"
#include "engine.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/filesystem/file_system.hpp"

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

    LOG_INFO << "Working directory: " << executablePath;
    fs::current_path(executablePath);

    projectSettings.projectVersion = version.string();
}

DefaultApplication::~DefaultApplication() {
    serialise();
}

void DefaultApplication::onStart() {
    FileSystem::Get()->mount(executablePath / "engine" / "assets" / "shaders", "EngineShaders");

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
    auto deviceManager = DeviceManager::Get();
    auto window = deviceManager->createWindow(windowInfo);

    window->OnClose().connect<&Engine::requestClose>(Engine::Get());

    // Win32 : Sets up a console window and redirects standard output to it
#if FUSION_PLATFORM_WINDOWS
    // Create console if needed
    if (projectSettings.isShowConsole) {
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        FILE* stream;
        freopen_s(&stream, "CONIN$", "r", stdin);
        freopen_s(&stream, "CONOUT$", "w+", stdout);
        freopen_s(&stream, "CONOUT$", "w+", stderr);
        SetConsoleTitle(TEXT(windowInfo.title.c_str()));
    }
#endif
}

void DefaultApplication::openNewProject(const fs::path& path, const std::string& name) {
    projectSettings.projectRoot = path / name;
    projectSettings.projectName = name;

    if (!fs::exists(projectSettings.projectRoot))
        fs::create_directory(projectSettings.projectRoot);

    mountPaths();

    // Set Default values
    projectSettings.projectVersion = version.string();
    projectSettings.title = name;
    if (auto window = DeviceManager::Get()->getWindow(0)) {
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

    auto assetPath = projectSettings.projectRoot / "assets";
    if (!fs::exists(assetPath))
        fs::create_directory(assetPath);

    auto scriptPath = assetPath / "scripts";
    if (!fs::exists(scriptPath))
        fs::create_directory(scriptPath);

    auto shaderPath = assetPath / "shaders";
    if (!fs::exists(shaderPath))
        fs::create_directory(shaderPath);

    auto scenePath = assetPath / "scenes";
    if (!fs::exists(scenePath))
        fs::create_directory(scenePath);

    auto texturePath = assetPath / "textures";
    if (!fs::exists(texturePath))
        fs::create_directory(texturePath);

    auto meshPath = assetPath / "meshes";
    if (!fs::exists(meshPath))
        fs::create_directory(meshPath);

    auto soundPath = assetPath / "sounds";
    if (!fs::exists(soundPath))
        fs::create_directory(soundPath);

    mountPaths();

    //m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
    //m_SceneManager->SwitchScene(0);
    //m_SceneManager->ApplySceneSwitch();

    projectLoaded = true;

    serialise();
}

void DefaultApplication::openProject(const fs::path& path) {
    projectSettings.projectName = path.filename().replace_extension().string();
    projectSettings.projectRoot = path.parent_path();

    //m_SceneManager = CreateUniquePtr<SceneManager>();

    deserialise();

    //m_SceneManager->LoadCurrentList();
    //m_SceneManager->ApplySceneSwitch();
}

void DefaultApplication::serialise() {
    std::stringstream is;
    {
        // output finishes flushing its contents when it goes out of scope
        cereal::JSONOutputArchive output{is};
        output(*this);
    }

    auto filePath = projectSettings.projectRoot / projectSettings.projectName;
    filePath += ".fsproj";

    LOG_INFO << "Serialising application: " << filePath;

    auto jsonStr = is.str();
    FileSystem::Write(filePath, jsonStr.data(), jsonStr.length());
}

void DefaultApplication::deserialise() {
    if (projectSettings.projectRoot.empty() || projectSettings.projectName.empty()) {
        LOG_INFO << "No saved Project file found";
        return;
    }

    auto filePath = projectSettings.projectRoot / projectSettings.projectName;
    filePath += ".fsproj";

    if (!fs::exists(filePath)) {
        LOG_INFO << "No saved Project file found: " << filePath;
        return;
    }

    mountPaths();

    std::string data = FileSystem::ReadText(filePath);
    std::istringstream is{data};
    try {
        cereal::JSONInputArchive input{is};
        input(*this);
    }
    catch (...) {
        // Set Default values
        projectSettings = {};
        projectSettings.projectVersion = version.string();

        //m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
        //m_SceneManager->SwitchScene(0);

        LOG_ERROR << "Failed to load project";
    }
}

void DefaultApplication::mountPaths() const {
    auto fs = FileSystem::Get();

    auto assetPath = projectSettings.projectRoot / "assets";
    fs->mount(assetPath, "Assets");
    fs->mount(assetPath / "meshes", "Meshes");
    fs->mount(assetPath / "textures", "Textures");
    fs->mount(assetPath / "sounds", "Sounds");
    fs->mount(assetPath / "scripts", "Scripts");
    fs->mount(assetPath / "shaders", "Shaders");
    fs->mount(assetPath / "scenes", "Scenes");
}