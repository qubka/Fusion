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
#include <cstring>
#include <string>
#include <deque>
#include <queue>
#include <array>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <future>
#include <list>
#include <mutex>
#include <numeric>
#include <random>
#include <any>
#include <iterator>
#include <type_traits>
#include <experimental/type_traits>
#include <new>
#include <tuple>
#include <filesystem>
namespace fs = std::filesystem;

#if __cplusplus >= 202002L
#include <span>
#else
#define TCB_SPAN_NAMESPACE_NAME std
#include <tcb/span.hpp>
#endif

// Math
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

// Serialzation
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

// Customization
#include <magic_enum.hpp>
namespace me = magic_enum;
#include <effolkronium/random.hpp>
// get base random alias which is auto seeded and has static API and internal state
using Random = effolkronium::random_static;
#include <bitmask/bitmask.hpp>

// Graphics
#include <volk/volk.h>

// ECS
#include <entt/entt.hpp>

// Engine
#include "fusion/core/base.h"
#include "fusion/core/log.h"
#include "fusion/core/profiler.h"

#include "fusion/utils/flat_map.h"
#include "fusion/utils/date_time.h"
#include "fusion/utils/elapsed_time.h"
#include "fusion/utils/enumerate.h"
#include "fusion/utils/glm_extention.h"
#include "fusion/utils/cereal_extention.h"
#include "fusion/utils/string.h"
#include "fusion/utils/vulkan.h"
#include "fusion/utils/vector.h"

#include "fusion/input/codes.h"
#include "fusion/core/module.h"
#include "fusion/core/time.h"
