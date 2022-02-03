#pragma once

#ifndef POLOS_UTILS_TYPES_H
#define POLOS_UTILS_TYPES_H

#include <cstdint>
#include <limits>
#include <cfloat>

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

	using byte = uint8_t;

	using event_id = std::size_t;

	using cstring = char const *;
}

#endif /* POLOS_UTILS_TYPES_H */
