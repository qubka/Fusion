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
#include <iterator>
#include <filesystem>
namespace fs = std::filesystem;

#if LIBUUID_CPP20_OR_GREATER
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

// Customization
#include <magic_enum.hpp>
namespace me = magic_enum;

#include <effolkronium/random.hpp>

// get base random alias which is auto seeded and has static API and internal state
using Random = effolkronium::random_static;

#include <bitmask/bitmask.hpp>

// Engine
#include "fusion/core/base.hpp"
#include "fusion/core/log.hpp"