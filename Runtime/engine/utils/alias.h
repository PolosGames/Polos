#pragma once

#ifndef POLOS_CORE_UTILS_ALIAS_H_
#define POLOS_CORE_UTILS_ALIAS_H_

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

    using event_id  = int64;
    using string_id = int64;

    using cstring = const char*;

    inline constexpr float  operator""_ms(std::size_t time)	{ return 0.001f * time;		}
    inline constexpr float  operator""_us(std::size_t time)	{ return 0.001f * 0.001f * time;		}
    inline constexpr double operator""_ns(std::size_t time)	{ return 0.001 * 0.001 * 0.001 * time;	}
}

#endif /* POLOS_CORE_UTILS_ALIAS_H_ */