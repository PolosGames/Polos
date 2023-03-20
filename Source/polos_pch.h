#include "polos_config.h"
#include "polos/platform/platform_detection.h"

#if defined(POLOS_WIN)
#	include <Windows.h>
#	include <winnt.h>
#endif

#include <algorithm>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <utility>
#include <variant>
#include <ranges>
#include <iterator>
#include <compare>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include <string>
#include <vector>
#include <memory_resource>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <span>
#include <optional>
#include <bitset>

// glm stuff
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stduuid/uuid.h>

#include "polos/core/debug/plassert.h"
#include "polos/core/debug/profiling.h"
#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/core/log.h"
#include "polos/core/engine/engine_config.h"
#include "polos/utils/type_util.h"
#include "polos/containers/containers.h"

namespace pl = ::polos;
