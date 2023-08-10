if(ANDROID)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_ANDROID_KHR)
elseif(WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_MACOS_MVK)
else()
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XCB_KHR)
endif()
add_subdirectory(volk)
list(APPEND ALL_LIBS volk)