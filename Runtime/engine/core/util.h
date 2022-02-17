#pragma once

#ifndef POLOS_UTILS_UTIL_H
#define POLOS_UTILS_UTIL_H

#include "types.h"
#include "logger.h"

namespace polos
{
    // Polynomial rolling hash
	size_t string_hash(std::string const &str);

	template<typename T>
	size_t type_hash();
}

template<typename T>
size_t polos::type_hash()
{
	std::string name = typeid(T).name();

	// find the namespace colon
	size_t name_pos = name.find_last_of(':');
	if (name_pos != std::string::npos)
	{
		name = name.substr(name_pos + 1);
	}

	// remove compiler generated types for pointers
	name_pos = name.find(" * __ptr64");
	if (name_pos != std::string::npos)
	{
		name = name.substr(0, name_pos);
		name += "*";
	}

	static size_t th = string_hash(name);

	return th;
}

#endif /* POLOS_UTILS_UTIL_H */