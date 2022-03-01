#pragma once

#ifndef POLOS_UTILS_TYPES_H
#define POLOS_UTILS_TYPES_H

#include <cstdint>
#include <cstddef>

namespace polos
{
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;

	using uintptr = std::uintptr_t;
	using ptrdiff = std::ptrdiff_t;

	using byte = uint8;

	using event_id = uint64;

	using cstring = const char*;

	inline float  operator""_ms(std::size_t time)	{ return 0.001f * time;		}
	inline double operator""_us(std::size_t time)	{ return 0.001 * 0.001 * time;			}
	inline double operator""_ns(std::size_t time)	{ return 0.001 * 0.001 * 0.001 * time;	}
}

#endif /* POLOS_UTILS_TYPES_H */
