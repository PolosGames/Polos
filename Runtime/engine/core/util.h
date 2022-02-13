#pragma once

#ifndef POLOS_UTILS_UTIL_H
#define POLOS_UTILS_UTIL_H

#include "types.h"
#include "logger.h"

namespace polos
{
    // Polynomial rolling hash
	static size_t string_hash(std::string const &str);

	template<typename T>
	static size_t type_hash();
}
#endif /* POLOS_UTILS_UTIL_H */

#include "util-inl.h"