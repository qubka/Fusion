#include "asset_registry.h"
#include "asset_database.h"

#include "fusion/core/engine.h"
#include "fusion/filesystem/file_watcher.h"

using namespace fe;

AssetRegistry::AssetRegistry() {

}

AssetRegistry::~AssetRegistry() {
}

void AssetRegistry::releaseAll() {
    assets.clear();
}

void AssetRegistry::onStart() {
    assetDatabase = std::make_unique<AssetDatabase>("D:\\Fusion\\New Project");

    std::set<std::string> exts{".fbx", ".obj"};

    fileWatcher = std::make_unique<FileWatcher>("D:\\Fusion\\New Project\\assets\\meshes", [&](const fs::path& path, FileEvent event) {
        return onFileChanged(path, event);
    }, std::move(exts));
}

void AssetRegistry::onUpdate() {
    fileWatcher->update();
}

void AssetRegistry::onFileChanged(const fs::path& path, FileEvent event) {
    if (!fs::is_regular_file(path))
        return;

    switch (event) {
        case FileEvent::Init:
            initFile();
            break;
        case FileEvent::Created:

            break;
        case FileEvent::Modified:
            //
            break;
        case FileEvent::Erased:
            //
            break;
    }
}

void AssetRegistry::initFile(fs::path filepath) {
    filepath += ".meta";
    if (FileSystem::IsExists(filepath)) {
        //.. look like old file, check db
        auto key = uuids::uuid::from_string(FileSystem::ReadText(filepath));
        if (key.has_value()) {
            auto path = assetDatabase->getValue(key.value());
            // if not value invalid or not present, add a new entry to db
            if (!path.has_value() || path.value() != filepath) {
                assetDatabase->put(key.value(), filepath);
            }
        } else {
            // invalid uuid, try to recover, if path not exit, generate a new uuid
            key = assetDatabase->getKey(filepath);
            FileSystem::WriteText(filepath, uuids::to_string(key.has_value() ? key.value() : uuids::uuid{ uuid_random_generator() }));
        }
    } else {
        //.. new file
        // try to find value in db to get uuid
        auto key = assetDatabase->getKey(filepath);
        if (key.has_value()) {
            // if preset, get uiid
            FileSystem::WriteText(filepath, uuids::to_string(key.value()));
        } else {
            // if not, generate new uuid
            uuids::uuid uuid{ uuid_random_generator() };
            // generate metafile
            FileSystem::WriteText(filepath, uuids::to_string(uuid));
            // add to db
            assetDatabase->put(uuid, filepath);
        }
    }
}
