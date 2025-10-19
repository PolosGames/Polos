#pragma once

/* #undef ENABLE_PROFILING */
#define GUI_USE_IMGUI
/* #undef QUILL_DISABLE_NON_PREFIXED_MACROS */

#if defined(_WIN64)
#	define POLOS_WIN

#elif defined(__linux__)
#	if defined(__ANDROID__)
#		define POLOS_ANDROID
#	else
#		define POLOS_LINUX
#	endif
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if defined(TARGET_OS_MAC)
#		define POLOS_MACOS
#	elif defined(TARGET_OS_IPHONE)
#		define POLOS_IPHONE
#	endif
#else
#	error Platform not supported by Polos.
#endif

#if defined(__clang__) && (__clang_major__ >= 13)
#   define POLOS_CLANG
#elif defined(__GNUC__) && (__GNUC__ >= 11)
#   define POLOS_GNUC
#elif defined(_MSC_VER) && (_MSC_VER >= 1900)
#   define POLOS_MSC
#endif
