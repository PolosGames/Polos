#pragma once

#include "polos_config.h"

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
#include <limits>
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

#ifdef _MSC_VER
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT
#endif

#ifdef POLOS_BUILD
    #define PLAPI DLL_EXPORT
#else
    #define PLAPI DLL_IMPORT
#endif
