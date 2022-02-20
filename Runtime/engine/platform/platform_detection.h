#pragma once

#ifndef POLOS_UTILS_PLATFORMDEFINES_H_
#define POLOS_UTILS_PLATFORMDEFINES_H_

#if defined(_WIN64)
#	define POLOS_WIN

#	include <Windows.h>
#	include <winnt.h>

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

#endif /* POLOS_UTILS_PLATFORMDEFINES_H_ */
