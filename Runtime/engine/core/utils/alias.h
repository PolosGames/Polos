#pragma once

#ifndef POLOS_CORE_UTILS_ALIAS_H_
#define POLOS_CORE_UTILS_ALIAS_H_

#include <cstdint>
#include <cstddef>

#include <vector>
#include <unordered_map>

namespace polos
{
	using uint8  = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	using int8  = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;

	using uintptr = std::uintptr_t;
	using ptrdiff = std::ptrdiff_t;

	using byte = uint8;

	using event_id  = uint64;
	using string_id = int64;

	using cstring = const char*;

	inline constexpr float  operator""_ms(std::size_t time)	{ return 0.001f * time;		}
	inline constexpr float  operator""_us(std::size_t time)	{ return 0.001f * 0.001f * time;		}
	inline constexpr double operator""_ns(std::size_t time)	{ return 0.001 * 0.001 * 0.001 * time;	}

	template<typename T>
	using DArray = std::vector<T>;

	template<typename Key, typename Value>
	using HashMap = std::unordered_map<Key, Value>;
}

#endif /* POLOS_CORE_UTILS_ALIAS_H_ */
