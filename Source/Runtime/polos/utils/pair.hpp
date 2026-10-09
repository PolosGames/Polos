///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_PAIR_HPP
#define POLOS_UTILS_PAIR_HPP

#include <utility>

namespace polos::utils
{

template<typename F, typename S>
struct Pair
{
    F first;
    S second;
};

template<typename F, typename S>
Pair(F, S) -> Pair<F, S>;

template<typename F, typename S>
constexpr auto MakePair(F& tFirst, S& tSecond) -> Pair<F, S>
{ return Pair<F, S>{.first = tFirst, .second = tSecond}; }

template<typename F, typename S>
constexpr auto MakePair(F&& tFirst, S&& tSecond) -> Pair<F, S>
{ return Pair<F, S>{.first = std::forward<F>(tFirst), .second = std::forward<S>(tSecond)}; }


}// namespace polos::utils

#endif// POLOS_UTILS_PAIR_HPP
