#include "asset_registry.h"

#include "fusion/core/engine.h"

using namespace fe;

AssetRegistry* AssetRegistry::Instance = nullptr;

AssetRegistry::AssetRegistry() {
    Instance = this;
}

AssetRegistry::~AssetRegistry() {
    Instance = nullptr;
}

void AssetRegistry::releaseAll() {
    assets.clear();

    // TODO: Move to reload

    const auto& path = Engine::Get()->getApp()->getProjectSettings().projectRoot;

    assetDatabase = std::make_unique<AssetDatabase>(path);

#if !FUSION_VIRTUAL_FS
    std::set<std::string> exts{".fbx", ".obj", ".png", ".meta"};

    fileWatcher = std::make_unique<FileWatcher>(path, [&](const fs::path& path, FileEvent event) {
        return onFileChanged(path, event);
    }, std::move(exts));
#endif
}

void AssetRegistry::onStart() {
}

void AssetRegistry::onUpdate() {
#if !FUSION_VIRTUAL_FS
    if (fileWatcher)
        fileWatcher->update();
#endif
}

void AssetRegistry::onStop() {
}

#if !FUSION_VIRTUAL_FS
void AssetRegistry::onFileChanged(const fs::path& path, FileEvent event) {
    if (!fs::is_regular_file(path))
        return;

    switch (event) {
        case FileEvent::Init:
        case FileEvent::Created:
            if (path.extension().string() != ".meta")
                onFileInit(path);
            break;
        case FileEvent::Modified:
            onFileModified(path);
            break;
        case FileEvent::Erased:
            onFileErased(path);
            break;
    }
}

void AssetRegistry::onFileInit(const fs::path& filepath) {
    // get full path to metadata
    fs::path metaPath { filepath };
    metaPath += ".meta";
    // get short path in project folder
    fs::path shortPath{ filepath.lexically_relative(Engine::Get()->getApp()->getProjectSettings().projectRoot) };

	// Is meta valid ?
    if (fs::exists(metaPath)) {
        // look like old file, check db
        auto uuid = uuids::uuid::from_string(FileSystem::ReadText(metaPath));
        if (uuid.has_value()) {
            auto path = assetDatabase->getValue(*uuid);
            if (!path.has_value()) {
                assetDatabase->put(*uuid, shortPath);
                FE_LOG_DEBUG("[{}] Path not present, add a new entry to db! New: '{}'", *uuid, shortPath);
            } else if (*path != shortPath) {
                assetDatabase->put(*uuid, shortPath, true);
                FE_LOG_DEBUG("[{}] Path not valid, overwrite current one! Old: '{}', New: '{}'", *uuid, *path, shortPath);
            }
            return; // exit
        }
        FE_LOG_DEBUG("Invalid uuid in '{}', try to recover, if path not exist, will generate a new uuid!", metaPath);
    }
    
    // new file |
    // try to find value in db to get uuid
    auto uuid = assetDatabase->getKey(shortPath);
    if (uuid.has_value()) {
        FileSystem::WriteText(metaPath, uuids::to_string(*uuid));
        FE_LOG_DEBUG("[{}] Write uuid to metafile back for: '{}'", *uuid, shortPath);
    } else {
        uuids::uuid id{ uuid_random_generator() };
        while (assetDatabase->getValue(id).has_value()) {
            id = { uuid_random_generator() };
        }
        FileSystem::WriteText(metaPath, uuids::to_string(id));
        assetDatabase->put(id, shortPath);
        FE_LOG_DEBUG("Generate new uuid for: '{}'", shortPath);
    }
}

void AssetRegistry::onFileModified(const fs::path& filepath) {
	// is it meta file?
	if (filepath.extension().string() == ".meta") {
		// get path to main file
		fs::path shortPath{ filepath };
		shortPath.replace_extension("");
		
		// if not valid, skip
		if (!fs::exists(shortPath))
			return;

		// get short path in project folder
		shortPath = shortPath.lexically_relative(Engine::Get()->getApp()->getProjectSettings().projectRoot);
		
		// look like old file, check db
        auto uuid = uuids::uuid::from_string(FileSystem::ReadText(filepath));
        if (uuid.has_value()) {
            auto path = assetDatabase->getValue(*uuid);
            if (!path.has_value()) {
                assetDatabase->put(*uuid, shortPath);
                FE_LOG_DEBUG("[{}] Path not present, add a new entry to db! New: '{}'", *uuid, shortPath);
            } else if (path != shortPath) {
                assetDatabase->put(*uuid, shortPath, true);
                FE_LOG_DEBUG("[{}] Path not valid, overwrite current one! Old: '{}', New: '{}'", *uuid, *path, shortPath);
            }
        } else {
            FE_LOG_DEBUG("Invalid uuid in '{}', try to recover!", filepath);
            
            // try to find value in db to get uuid
			uuid = assetDatabase->getKey(shortPath);
			if (uuid.has_value()) {
				FileSystem::WriteText(filepath, uuids::to_string(*uuid));
                FE_LOG_DEBUG("[{}] Write uuid to metafile back for: '{}'", *uuid, shortPath);
			} else {
                FE_LOG_DEBUG("Cannot recover uuid for '{}'", shortPath);
			}
		}
	} 
	// Regular file
	else {
		// get full path to metadata
		fs::path metaPath { filepath };
		metaPath += ".meta";

        // get short path in project folder
        fs::path shortPath{ filepath.lexically_relative(Engine::Get()->getApp()->getProjectSettings().projectRoot) };

        // Is meta valid ?
        if (fs::exists(metaPath)) {
            // look like old file, check db
            auto uuid = uuids::uuid::from_string(FileSystem::ReadText(metaPath));
            if (uuid.has_value()) {
                auto path = assetDatabase->getValue(*uuid);
                if (!path.has_value()) {
                    FE_LOG_DEBUG("[{}] Path not present, add a new entry to db! New: '{}'", *uuid, shortPath);
                    goto recover;
                } else if (path != shortPath) {
                    FE_LOG_DEBUG("[{}] Path not valid! Old: '{}', New: '{}'", *uuid, *path, shortPath);
                    goto recover;
                }

                // reload asset
                FE_LOG_DEBUG("[{}] Asset reload: '{}'", *uuid, shortPath);

                return;
            }

recover:
            FE_LOG_DEBUG("Invalid uuid in '{}', try to recover!", metaPath);
        }

        // try to find value in db to get uuid
        auto uuid = assetDatabase->getKey(shortPath);
        if (uuid.has_value()) {
            FileSystem::WriteText(metaPath, uuids::to_string(*uuid));
            FE_LOG_DEBUG("[{}] Write uuid to metafile back for: '{}'", *uuid, shortPath);
        } else {
            FE_LOG_DEBUG("Cannot recover uuid for '{}'", shortPath);
        }
	}
}

void AssetRegistry::onFileErased(const fs::path& filepath) {

}
#endif