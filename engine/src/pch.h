#pragma once

// Standart
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
namespace fs = std::filesystem;

#if __cplusplus >= 202002L
#include <span>
#else
#include <gsl/span>
#endif

#include <uuid.h>

// Math
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
//#include <glm/gtx/hash.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

// Serialzation
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

// Customization
#include <fmt/core.h>
#include <fmt/format.h>
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

#include "fusion/utils/flat_map.h"
#include "fusion/utils/date_time.h"
#include "fusion/utils/elapsed_time.h"
#include "fusion/utils/enumerate.h"
#include "fusion/utils/glm_extention.h"
#include "fusion/utils/cereal_extention.h"
#include "fusion/utils/format_extention.h"
#include "fusion/utils/string.h"
#include "fusion/utils/vulkan.h"
#include "fusion/utils/vector.h"

// TEMP ?
#include "fusion/input/codes.h"
#include "fusion/core/module.h"
#include "fusion/core/time.h"
