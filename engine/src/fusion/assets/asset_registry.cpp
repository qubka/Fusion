#include "asset_registry.h"

#include "fusion/core/engine.h"

using namespace fe;

AssetRegistry::~AssetRegistry() {
}

void AssetRegistry::releaseAll() {
    assets.clear();
}
