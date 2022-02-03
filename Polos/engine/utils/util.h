#pragma once

#ifndef POLOS_UTILS_UTIL_H
#define POLOS_UTILS_UTIL_H

#include "types.h"
#include <functional>
#include "logger.h"

namespace polos
{
	static size_t string_hash(const std::string &str)
	{
		static size_t h = std::hash<std::string>{}(str);
		return h;
	}

	template<typename T>
	static size_t type_hash()
	{
		std::string name = typeid(T).name();
		static size_t th = std::hash<std::string>{}(name);
		return th;
	}
}

#endif /* POLOS_UTILS_UTIL_H */
