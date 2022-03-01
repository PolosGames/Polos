#pragma once
#ifndef POLOS_CORE_META_H_
#define POLOS_CORE_META_H_

#include <type_traits>

namespace polos
{
	/**
	 * \brief Checks whether or not a class has a static OnInitialize function
	 * \tparam T The class to have the OnInitialize function
	 */
	template <typename T, typename = void>
	constexpr bool has_OnInit{};

	template <typename T>
	constexpr bool has_OnInit<T, std::void_t<decltype(T::OnInitialize())>> = true;

	/**
	 * \brief Checks whether or not a class has a static OnShutdown function
	 * \tparam T The class to have the OnShutdown function
	 */
	template <typename T, typename = void>
	constexpr bool has_OnShutdown{};

	template <typename T>
	constexpr bool has_OnShutdown<T, std::void_t<decltype(T::OnShutdown())>> = true;
	
}

#endif /* POLOS_CORE_META_H_ */