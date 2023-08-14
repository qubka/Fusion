#include "asset_registry.h"

#include "fusion/core/engine.h"

using namespace fe;

AssetRegistry::AssetRegistry() {

}

AssetRegistry::~AssetRegistry() {
}

void AssetRegistry::releaseAll() {
    assets.clear();

    // TODO: Move to reload

    std::set<std::string> exts{".fbx", ".obj", ".png", ".meta"};

    const auto& path = Engine::Get()->getApp()->getProjectSettings().projectRoot;

    assetDatabase = std::make_unique<AssetDatabase>(path);
    fileWatcher = std::make_unique<FileWatcher>(path, [&](const fs::path& path, FileEvent event) {
        return onFileChanged(path, event);
    }, std::move(exts));
}

void AssetRegistry::onUpdate() {
    if (fileWatcher)
        fileWatcher->update();
}

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
            //
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
    if (fs::exist(metaPath)) {
        //.. look like old file, check db
        auto uuid = uuids::uuid::from_string(FileSystem::ReadText(metaPath));
        if (uuid.has_value()) {
            auto path = assetDatabase->getValue(uuid.value());
            if (!path.has_value()) {
                // if not present, add a new entry to db
                assetDatabase->put(uuid.value(), shortPath);
            } else if (path.value() != shortPath) {
                // if value not valid, overwrite old one
                assetDatabase->put(uuid.value(), shortPath, true);
            }
            return; // exit
        }
        // else -> invalid uuid, try to recover, if path not exit, generate a new uuid
    }

    //.. new file |
    // try to find value in db to get uuid
    auto uuid = assetDatabase->getKey(shortPath);
    if (uuid.has_value()) {
        // if preset, write uuid to metafile back
        FileSystem::WriteText(metaPath, uuids::to_string(uuid.value()));
    } else {
        // if not, generate new uuid
        uuids::uuid id{ uuid_random_generator() };
        while (assetDatabase->getValue(id).has_value()) {
            id = { uuid_random_generator() };
        }
        // generate metafile
        FileSystem::WriteText(metaPath, uuids::to_string(id));
        // add to db
        assetDatabase->put(id, shortPath);
    }
}

void AssetRegistry::onFileModified(const fs::path& filepath) {
	// is it meta file?
	if (filepath.extension.string() == ".meta") {
		// get path to main file
		fs::path shortPath{ filepath };
		shortPath.replace_extension(".meta", "");
		
		// if not valid, skip
		if (!fs::exist(shortPath))
			return;

		// get short path in project folder
		shortPath = shortPath.lexically_relative(Engine::Get()->getApp()->getProjectSettings().projectRoot)
		
		//.. look like old file, check db
        auto uuid = uuids::uuid::from_string(FileSystem::ReadText(filepath));
        if (uuid.has_value()) {
            auto path = assetDatabase->getValue(uuid.value());
            if (!path.has_value()) {
                // if not present, add a new entry to db
                assetDatabase->put(uuid.value(), shortPath);
            } else if (path.value() != shortPath) {
                // if value not valid, overwrite old one
                assetDatabase->put(uuid.value(), shortPath, true);
            }
        } else {
			 // try to find value in db to get uuid
			auto uuid = assetDatabase->getKey(shortPath);
			if (uuid.has_value()) {
				// if preset, write uuid to metafile back
				FileSystem::WriteText(fullPath, uuids::to_string(uuid.value()));
			} else {
				// if not, (How it can be possible ?)
			}
		}
	} 
	// Regular file
	else {
		// get full path to metadata
		fs::path metaPath { filepath };
		metaPath += ".meta";
		
		// if not valid, skip (How it can be possible?)
		if (!fs::exist(metaPath))
			return;
		
		// get short path in project folder
		fs::path shortPath{ filepath.lexically_relative(Engine::Get()->getApp()->getProjectSettings().projectRoot) };

		//.. look like old file, check db
        auto uuid = uuids::uuid::from_string(FileSystem::ReadText(metaPath));
        if (uuid.has_value()) {
            auto path = assetDatabase->getValue(uuid.value());
            if (!path.has_value()) {
                // if not, (How it can be possible ?)
            } else if (path.value() != shortPath) {
                // if not, (How it can be possible ?)
            }
			
			// reload asset with given uuid
			
        } else {
			// try to find value in db to get uuid
			auto uuid = assetDatabase->getKey(shortPath);
			if (uuid.has_value()) {
				// if preset, write uuid to metafile back
				FileSystem::WriteText(fullPath, uuids::to_string(uuid.value()));
			} else {
				// if not, (How it can be possible ?)
			}
		}
	}
}