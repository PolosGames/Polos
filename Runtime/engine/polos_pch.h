#include "platform_detection.h"

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

#include "debug/plassert.h"
#include "debug/profiling.h"
#include "utils/alias.h"
#include "utils/feature.h"
#include "core/log.h"
