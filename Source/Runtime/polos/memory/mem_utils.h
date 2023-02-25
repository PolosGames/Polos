#pragma once

#include "polos/utils/alias.h"

namespace polos::memory
{
    struct internal_buffer
    {
        byte* buffer{};
        size_t bufferSize{};
    };
    
    class MemUtils
    {
    public:
        static uint32 constexpr k_MemoryAlignment = 2 * sizeof(void*);

        static bool IsPowerOfTwo(uintptr p_Ptr);
        static bool IsAligned(uintptr p_Ptr);
        static uint32 CalculatePadding(uintptr p_Ptr);
    };

    inline bool MemUtils::IsPowerOfTwo(uintptr p_Ptr)
    {
        return (p_Ptr & (p_Ptr - 1)) == 0;
    }

    inline bool MemUtils::IsAligned(uintptr p_Ptr)
    {
        // TODO: either make k_MemoryAlignment uint64, or find another solution!!!
        return (p_Ptr & static_cast<uintptr>(k_MemoryAlignment - 1)) == 0;
    }

    inline uint32 MemUtils::CalculatePadding(uintptr p_Ptr)
    {
        return k_MemoryAlignment - static_cast<uint32>(p_Ptr & static_cast<uintptr>(k_MemoryAlignment - 1));
    }
} // namespace polos
