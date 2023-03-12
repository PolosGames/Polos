#pragma once

#include "polos/core/log.h"
#include "polos/platform/platform_detection.h"
#include "polos/utils/macro_util.h"

#if defined(PL_DEBUG)
#   if defined(POLOS_MSC)
#       define DEBUGBRK __debugbreak()
#   elif defined(POLOS_CLANG) || defined(POLOS_GNUC)
#       include <assert.h>
#       define DEBUGBRK assert(0);
#   endif
#   define PL_ASSERT(check, ...)                                                             \
        {                                                                                    \
            if (!(check))                                                                    \
            {                                                                                \
                std::string assert_str{__VA_ARGS__};                                         \
                LOG_ENGINE_CRITICAL("Assertion failed at File: {0}, Line: {1}. \n Msg: {2}", \
                    __FILE__,                                                                \
                    __LINE__,                                                                \
                    assert_str.c_str());                                                     \
                DEBUGBRK;                                                                    \
            }                                                                                \
        }
#   define STATIC_ASSERT(check) static_assert(check, "Compile time assertion fail: " PL_STRINGIFY(check))
#else
#   define DEBUGBRK
#   define PL_ASSERT(check, ...)   PL_VOID_CAST(check)
#   define STATIC_ASSERT(check) PL_VOID_CAST(check)
#endif
