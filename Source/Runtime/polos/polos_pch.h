#include "platform/platform_detection.h"

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

#include "core/debug/plassert.h"
#include "core/debug/profiling.h"
#include "utils/alias.h"
#include "utils/feature.h"
#include "core/log.h"
#include "core/engine/engine_config.h"

namespace pl = ::polos;
