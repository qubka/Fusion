#pragma once

// Standart
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <cstring>
#include <string>
#include <deque>
#include <queue>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <typeindex>
#include <list>
#include <mutex>
#include <numeric>
#include <random>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <magic_enum.hpp>
namespace me = magic_enum;

#if defined(__ANDROID__)
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include "android.hpp"
#endif

#include "fusion/core/base.hpp"
#include "fusion/core/common.hpp"
#include "fusion/core/log.hpp"
#include "fusion/utils/extensions.hpp"