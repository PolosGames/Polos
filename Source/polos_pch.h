#pragma once

#include "polos_config.h"
#include "polos/platform/platform_detection.h"

#if defined(POLOS_WIN)
#	include <Windows.h>
#	include <winnt.h>
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <numeric>
#include <optional>
#include <random>
#include <ranges>
#include <queue>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

// glm stuff
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "polos/core/debug/plassert.h"
#include "polos/core/debug/profiling.h"
#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/core/log.h"
#include "polos/core/engine/engine_config.h"
#include "polos/utils/type_util.h"
#include "polos/containers/containers.h"

namespace pl = ::polos;
