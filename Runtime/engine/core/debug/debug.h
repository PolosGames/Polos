#pragma once
#ifndef POLOS_CORE_DEBUG_H
#define POLOS_CORE_DEBUG_H

#include "utils/macro_util.h"

#ifdef PL_DEBUG
#	define ASSERTSTR(check, ...)		{ if (!(check)) { LOG_CORE_CRITICAL("Assertion failed at File: {0}, Line: {1}. \n Msg: {2}", __FILE__, __LINE__, __VA_ARGS__); __debugbreak();} }
#	define ASSERT(check)				ASSERTSTR(check, "None")
#	define STATIC_ASSERT(check)			static_assert(check, "Compile time assertion fail: " PL_STRINGIFY(check))
#else
#	define ASSERT(check, ...)
#	define ASSERT(check)
#	define STATIC_ASSERT(check)
#endif

#endif /* POLOS_CORE_DEBUG_H */