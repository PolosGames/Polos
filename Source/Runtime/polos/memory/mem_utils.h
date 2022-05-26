#pragma once
#ifndef POLOS_CORE_MEMORY_MEMUTILS_H_
#define POLOS_CORE_MEMORY_MEMUTILS_H_

#include "polos/utils/alias.h"

namespace polos::memory
{
    struct InternalBuffer
    {
        byte* buffer;
        size_t bufferSize;
    };
    
    class MemUtils
    {
    public:
        static uint32 constexpr kMemoryAlignment = 2 * sizeof(void*);

        static bool IsPowerOfTwo(uintptr p);
        static bool IsAligned(uintptr p);
        static uint32 CalculatePadding(uintptr p);
    };

    inline bool MemUtils::IsPowerOfTwo(uintptr p)
    {
        return (p & (p - 1)) == 0;
    }

    inline bool MemUtils::IsAligned(uintptr p)
    {
        return (p & (kMemoryAlignment - 1)) == 0;
    }

    inline uint32 MemUtils::CalculatePadding(uintptr p)
    {
        return kMemoryAlignment - (p & (kMemoryAlignment - 1));
    }
} // namespace polos

#endif /* POLOS_CORE_MEMORY_MEMUTILS_H_ */