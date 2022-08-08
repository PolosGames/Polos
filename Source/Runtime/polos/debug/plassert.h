#pragma once
#ifndef POLOS_CORE_DEBUG_H_
#define POLOS_CORE_DEBUG_H_

#include "polos/utils/macro_util.h"
#include "polos/core/log.h"
#include "polos/platform/platform_detection.h"

#ifdef PL_DEBUG
#   if defined(POLOS_MSC)
#       define DEBUGBRK __debugbreak()
#   elif defined(POLOS_CLANG) || defined(POLOS_GNUC)
#include <assert.h>
#       define DEBUGBRK assert(0);
#   endif
#	define ASSERTSTR(check, ...)    { if (!(check)) { LOG_ENGINE_CRITICAL("Assertion failed at File: {0}, Line: {1}. \n Msg: {2}", __FILE__, __LINE__, __VA_ARGS__); DEBUGBRK;} }
#	define ASSERT(check)            ASSERTSTR(check, "None")
#	define STATIC_ASSERT(check)	    static_assert(check, "Compile time assertion fail: " PL_STRINGIFY(check))
#else
#   define DEBUGBRK
#	define ASSERTSTR(check, ...)
#	define ASSERT(check)
#	define STATIC_ASSERT(check)
#endif

#endif /* POLOS_CORE_DEBUG_H_ */