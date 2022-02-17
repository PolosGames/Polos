#pragma once
#ifndef POLOS_CORE_DEBUG_H
#define POLOS_CORE_DEBUG_H


#ifdef PL_DEBUG
#	define ASSERT_S(check, ...)				{ if (!(check)) { LOG_CORE_CRITICAL("Assertion failed at File: {0}, Line: {1}. \n Msg: {2}", __FILE__, __LINE__, __VA_ARGS__); __debugbreak();} }
#	define ASSERT(check)					ASSERT_S(check, "None")
#else
#	define ASSERT_S(check, ...)
#	define ASSERT(check)
#endif

#include <optick.h>

#define PROFILE_FRAME(FRAME_NAME, ...)	OPTICK_FRAME(FRAME_NAME, __VA_ARGS__)
#define PROFILE_THREAD(...)				OPTICK_THREAD(__VA_ARGS__)
#define PROFILE_FUNC(...)				OPTICK_EVENT(__VA_ARGS__)
#define PROFILE_TAG(TAG_NAME, ...)		OPTICK_TAG(TAG_NAME, __VA_ARGS__)

#if 0
#	define PROFILE_FRAME(FRAME_NAME, ...)
#	define PROFILE_THREAD(...)
#	define PROFILE_FUNC(...)
#	define PROFILE_TAG(TAG_NAME, ...)
#endif

#endif /* POLOS_CORE_DEBUG_H */