#pragma once

#ifndef POLOS_UTILS_TYPES_H
#define POLOS_UTILS_TYPES_H

namespace polos
{
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using byte = uint8;

	using event_id = uint64_t;

	using cstring = char const *;
	using string = std::string;
	using string_view = std::string_view;
}

#endif /* POLOS_UTILS_TYPES_H */
