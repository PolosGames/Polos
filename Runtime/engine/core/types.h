#pragma once

#ifndef POLOS_UTILS_TYPES_H
#define POLOS_UTILS_TYPES_H

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
}

#endif /* POLOS_UTILS_TYPES_H */