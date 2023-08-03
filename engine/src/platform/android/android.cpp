#include "android.h"

#include <android/configuration.h>

int32_t fe::android::screenDensity{ 0 };
android_app* fe::android::androidApp{ nullptr };

void fe::android::getDeviceConfig(AAssetManager* assetManager) {
    // Screen density
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, assetManager);
    fe::android::screenDensity = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
}