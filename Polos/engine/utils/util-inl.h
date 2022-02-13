#pragma once

#ifndef POLOS_UTILS_UTILINL_H
#define POLOS_UTILS_UTILINL_H

namespace polos
{
	size_t string_hash(std::string const &str)
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

	template<typename T>
	size_t type_hash()
	{
		std::string name = typeid(T).name();

		size_t name_pos = name.find_last_of(':');

		if (name_pos != std::string::npos)
		{
			name = name.substr(name_pos + 1);
		}

		static size_t th = string_hash(name);

		return th;
	}
}

#endif /* POLOS_UTILS_UTILINL_H */