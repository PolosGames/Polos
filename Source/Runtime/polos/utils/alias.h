#pragma once

#include <cstdint>
#include <cstddef>

namespace polos
{
    using uint8  = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    
    using ulong32 = unsigned long;
    using ulong64 = unsigned long long;

    using int8  = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;
    
    using long32 = long;
    using long64 = long long;

    using uintptr = std::uintptr_t;
    using ptrdiff = std::ptrdiff_t;

    using byte = uint8;

    using StringId       = int64;
    using ResourceHandle = uint64;

    using cstring = const char*;
}

inline constexpr float operator""_ms(long double time) { return 0.001f * static_cast<float>(time); }
inline constexpr float operator""_us(long double time) { return 0.001f * 0.001f * static_cast<float>(time); }
