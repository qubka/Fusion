#include "android.hpp"

#if defined(__ANDROID__)
#include <android/configuration.h>

int32_t vkx::android::screenDensity{ 0 };
android_app* vkx::android::androidApp{ nullptr };

void vkx::android::getDeviceConfig(AAssetManager* assetManager) {
    // Screen density
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, assetManager);
    vkx::android::screenDensity = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
}
#endif
