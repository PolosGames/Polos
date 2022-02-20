#pragma once
#ifndef POLOS_CORE_META_H_
#define POLOS_CORE_META_H_



namespace polos
{
	template <typename T, typename = void>
	constexpr bool has_init{};

	template <typename T>
	constexpr bool has_init<T, std::void_t<decltype(T::init())>> = true;
}

#endif /* POLOS_CORE_META_H_ */