#pragma once
#ifndef POLOS_CORE_UTILS_FEATURE_H_
#define POLOS_CORE_UTILS_FEATURE_H_

#if defined(__has_cpp_attribute)
#	define PL_HAS_CPP_ATTR(x) __has_cpp_attribute(x)
#else
#   define PL_HAS_CPP_ATTR(x) 0
#endif

#if PL_HAS_CPP_ATTR(maybe_unused)
#	define PL_MAYBEUNUSED [[maybe_unused]]
#else
#	define PL_MAYBEUNUSED
#endif

#if PL_HAS_CPP_ATTR(nodiscard)
#	define PL_NODISCARD_MSG(Reason) [[nodiscard(Reason)]]
#	define PL_NODISCARD             PL_NODISCARD_MSG("None")
#else
#	define PL_NODISCARD_MSG(Reason)
#	define PL_NODISCARD
#endif

#if PL_HAS_CPP_ATTR(likely)
#	define PL_LIKELY [[likely]]
#else
#	define PL_LIKELY
#endif

#if PL_HAS_CPP_ATTR(unlikely)
#	define PL_UNLIKELY [[unlikely]]
#else
#	define PL_UNLIKELY
#endif

#endif /* POLOS_CORE_UTILS_FEATURE_H_ */