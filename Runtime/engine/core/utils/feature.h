#pragma once
#ifndef POLOS_CORE_UTIL_FEATURE_H_
#define POLOS_CORE_UTIL_FEATURE_H_

#include <Windows.h>

#if __cplusplus >= 202002L
#	define PL_HAS_CPP_ATTRIB(attrib) __has_cpp_attribute(attrib)
#else
#	define PL_HAS_CPP_ATTRIB(attrib) 0
#endif

#if PL_HAS_CPP_ATTRIB(maybe_unused)
#	define PL_MAYBEUNUSED [[maybe_unused]]
#else
#	define PL_MAYBEUNUSED
#endif

#if PL_HAS_CPP_ATTRIB(nodiscard)
#	define PL_NODISCARD [[nodiscard]]
#else
#	define PL_NODISCARD
#endif

#if PL_HAS_CPP_ATTRIB(likely)
#	define PL_LIKELY [[likely]]
#else
#	define PL_LIKELY
#endif

#if PL_HAS_CPP_ATTRIB(unlikely)
#	define PL_UNLIKELY [[unlikely]]
#else
#	define PL_UNLIKELY
#endif

#endif /* POLOS_CORE_UTIL_FEATURE_H_ */