#pragma once
#ifndef POLOS_CORE_UTILS_TYPEUTIL_H_
#define POLOS_CORE_UTILS_TYPEUTIL_H_

#include <string>

#include "alias.h"

namespace polos
{
	////////////////////		
	/// DECLARATIONS ///
	////////////////////

	template<typename T>
	std::string type_name()
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

		return name.c_str();
	}

	// Polynomial rolling hash
	size_t string_hash(std::string const& str);

	template<typename T>
	size_t type_hash()
	{
		static std::string name = type_name<T>();
		static size_t th = string_hash(name);

		return th;
	}

	///////////////////	
	/// DEFINITIONS ///
	///////////////////

	inline size_t string_hash(std::string const& str)
	{
		uint32 p = 31;
		uint32 m = 1'000'000'009;
		size_t power_of_p = 1;
		size_t hash_val = 0;

		// Loop to calculate the hash value
		// by iterating over the elements of string
		for (char c : str)
		{
			// make the character lowercase
			if (c >= 65 && c - 'a' < 0) c += 32;

			hash_val = (hash_val + (c - 'a' + 1) * power_of_p) % m;
			power_of_p = (power_of_p * p) % m;
		}

		//return positive remainder only
		return (hash_val % m + m) % m;
	}
}


#endif /* POLOS_CORE_UTILS_TYPEUTIL_H_ */