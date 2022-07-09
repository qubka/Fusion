#include "default_application.hpp"
#include "engine.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/filesystem/file_system.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#if FUSION_PLATFORM_WINDOWS
#include <windows.h>
#endif

using namespace fe;

DefaultApplication::DefaultApplication(std::string name) : Application{std::move(name)} {


#if FUSION_PLATFORM_MAC
    executablePath = std::filesystem::canonical("/proc/self/exe").parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();
#else
    executablePath = std::filesystem::canonical("/proc/self/exe").parent_path().parent_path().parent_path();
#endif
    LOG_INFO << "Working directory : " << executablePath.string();
    std::filesystem::current_path(executablePath);
}

DefaultApplication::~DefaultApplication() {

}

void DefaultApplication::onStart() {
    deserialise();

    WindowInfo windowInfo = {};
    windowInfo.size = projectSettings.viewport;
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

void DefaultApplication::openNewProject(const std::filesystem::path& path) {
    projectSettings.projectRoot = path / name;
    projectSettings.projectName = name;

    projectSettings.engineAssetPath = executablePath / "engine" / "assets";

    if (!std::filesystem::exists(projectSettings.projectRoot))
        std::filesystem::create_directory(projectSettings.projectRoot);

    // Set Default values
    projectSettings.viewport = { 1280, 720 };
    projectSettings.title = "Fusion App";
    projectSettings.isBorderless = false;
    projectSettings.isResizable = true;
    projectSettings.isFloating = false;
    projectSettings.isFullscreen = false;
    projectSettings.isVSync = false;
    projectSettings.isShowConsole = true;

    auto assetPath = projectSettings.projectRoot / "assets";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    auto scriptPath = assetPath / "scripts";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    auto scenePath = assetPath / "scenes";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    auto texturePath = assetPath / "textures";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    auto meshPath = assetPath / "meshes";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    auto soundPath = assetPath / "sounds";
    if (!std::filesystem::exists(assetPath))
        std::filesystem::create_directory(assetPath);

    //m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
    //m_SceneManager->SwitchScene(0);
    //m_SceneManager->ApplySceneSwitch();

    projectLoaded = true;

    serialise();
}

void DefaultApplication::openProject(const std::filesystem::path& path) {
    projectSettings.projectName = path.filename().replace_extension();
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

    LOG_INFO << "Serialising Application : " << filePath;

    FileSystem::WriteText(filePath, is.str());
}

void DefaultApplication::deserialise() {
    auto filePath = projectSettings.projectRoot / projectSettings.projectName;
    filePath += ".fsproj";

    if (!std::filesystem::exists(filePath)) {
        LOG_INFO << "No saved Project file found : " << filePath;
        {
            openNewProject(executablePath);
        }
        return;
    }

    std::string data = FileSystem::ReadText(filePath);
    std::istringstream is{data};
    try {
        cereal::JSONInputArchive input{is};
        input(*this);

    } catch (...) {
        // Set Default values
        projectSettings.projectVersion = version;
        projectSettings.title = "Fusion App";
        projectSettings.viewport = { 1280, 720 };
        projectSettings.isBorderless = false;
        projectSettings.isResizable = true;
        projectSettings.isFloating = false;
        projectSettings.isFullscreen = false;
        projectSettings.isVSync = false;
        projectSettings.isShowConsole = true;

        projectSettings.engineAssetPath = executablePath / "engine" / "assets";

        //m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
        //m_SceneManager->SwitchScene(0);

        LOG_ERROR << "Failed to load project";
    }
}
