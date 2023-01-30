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

#include "polos/debug/plassert.h"
#include "polos/debug/profiling.h"
#include "polos/utils/alias.h"
#include "polos/utils/feature.h"
#include "polos/core/log.h"
#include "polos/engine/engine_config.h"

namespace pl = ::polos;
