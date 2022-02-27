#pragma once
#ifndef POLOS_CORE_META_H_
#define POLOS_CORE_META_H_

#include <type_traits>

namespace polos
{
	/**
	 * \brief Checks whether or not a class has a static Initialize function
	 * \tparam T The class to have the Init function
	 */
	template <typename T, typename = void>
	constexpr bool has_init{};

	template <typename T>
	constexpr bool has_init<T, std::void_t<decltype(T::Initialize())>> = true;

	/**
	 * \brief Checks whether or not a class has a static Shutdown function
	 * \tparam T The class to have the Init function
	 */
	template <typename T, typename = void>
	constexpr bool has_shutdown{};

	template <typename T>
	constexpr bool has_shutdown<T, std::void_t<decltype(T::Shutdown())>> = true;
	
}

#endif /* POLOS_CORE_META_H_ */