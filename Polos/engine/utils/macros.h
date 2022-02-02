#pragma once

#ifndef POLOS_UTILS_MACROS_H
#define POLOS_UTILS_MACROS_H

#ifdef PL_DEBUG
#	define ASSERT(check, ...) { if (!(check)) { LOG_CRITICAL("Assertion failed at File: {0}, Line: {1}. \n msg: {2}", __FILE__, __LINE__, __VA_ARGS__); __debugbreak();} }
#else
#	define ASSERT(check, ...)
#endif

#endif /* POLOS_UTILS_MACROS_H */
