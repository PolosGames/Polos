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
#define BEGIN_PROFILING()				OPTICK_START_CAPTURE()
#define STOP_PROFILING()				OPTICK_STOP_CAPTURE()
#define SAVE_PROFILING(...)				OPTICK_SAVE_CAPTURE(__VA_ARGS__)
#define PROFILE_SCOPE(SCOPE_NAME)		OPTICK_EVENT(SCOPE_NAME)
#define PROFILE_FRAME(FRAME_NAME, ...)	OPTICK_FRAME(FRAME_NAME, __VA_ARGS__)
#define PROFILE_THREAD(THREAD_NAME)		OPTICK_THREAD(THREAD_NAME)
#define PROFILE_FUNC(...)				OPTICK_EVENT(__VA_ARGS__)
#define PROFILE_TAG(TAG_NAME, ...)		OPTICK_TAG(TAG_NAME, __VA_ARGS__)

#if 0
#	define PROFILE_FRAME(FRAME_NAME, ...)
#	define PROFILE_THREAD(...)
#	define PROFILE_FUNC(...)
#	define PROFILE_TAG(TAG_NAME, ...)
#endif

#endif /* POLOS_CORE_DEBUG_H */