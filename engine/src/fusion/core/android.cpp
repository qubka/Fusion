#include "android.hpp"

#if FUSION_PLATFORM_ANDROID
#include <android/configuration.h>

int32_t android::screenDensity{ 0 };
android_app* android::androidApp{ nullptr };

void android::getDeviceConfig(AAssetManager* assetManager) {
    // Screen density
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, assetManager);
    vkx::android::screenDensity = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
}
#endif