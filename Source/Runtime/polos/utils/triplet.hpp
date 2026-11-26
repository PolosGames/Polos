///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_TRIPLET_HPP
#define POLOS_UTILS_TRIPLET_HPP

#include <compare>
#include <utility>

namespace polos::utils
{

template<typename F, typename S, typename T>
struct Triplet
{
    F first;
    S second;
    T third;

    auto operator<=>(Triplet const&) const = default;
};

template<typename F, typename S, typename T>
Triplet(F, S, T) -> Triplet<F, S, T>;

template<typename F, typename S, typename T>
constexpr auto MakeTriplet(F& tFirst, S& tSecond, T& tThird) -> Triplet<F, S, T>
{
    return Triplet<F, S, T>{.first = tFirst, .second = tSecond, .third = tThird};
}

template<typename F, typename S, typename T>
constexpr auto MakeTriplet(F&& tFirst, S&& tSecond, T&& tThird) -> Triplet<F, S, T>
{
    return Triplet<F, S, T>{.first = std::forward<F>(tFirst), .second = std::forward<S>(tSecond), .third = std::forward<T>(tThird)};
}


}// namespace polos::utils

#endif// POLOS_UTILS_TRIPLET_HPP
