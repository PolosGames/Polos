///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_YAML_READ_HPP
#define POLOS_UTILS_YAML_READ_HPP

#include <ryml/ryml.hpp>

#include <array>
#include <charconv>
#include <istream>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace polos::utils::yaml
{

inline auto Parse(std::istream& tInput) -> ryml::Tree
{
    std::string const text{std::istreambuf_iterator<char>{tInput}, std::istreambuf_iterator<char>{}};
    return ryml::parse_in_arena({text.data(), text.size()});
}

inline auto Find(ryml::ConstNodeRef tNode, ryml::csubstr tKey) -> ryml::ConstNodeRef
{
    return tNode.readable() && tNode.is_map() ? tNode.find_child(tKey) : ryml::ConstNodeRef{};
}

inline auto Scalar(ryml::ConstNodeRef tNode, std::string_view tDefault = {}) -> std::string_view
{
    if (!tNode.readable() || !tNode.has_val() || tNode.val_is_null())
    {
        return tDefault;
    }

    auto const value = tNode.val();
    return {value.str, value.len};
}

template<typename T>
auto Read(ryml::ConstNodeRef tNode, T tDefault = {}) -> T
{
    auto value = Scalar(tNode);
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        return Scalar(tNode, tDefault);
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        bool parsed{};
        if (c4::from_chars({value.data(), value.size()}, &parsed))
        {
            return parsed;
        }
    }
    else
    {
        if (value.starts_with('+'))
        {
            value.remove_prefix(1U);
        }
        if (!value.empty())
        {
            T parsed{};
            auto const end = value.data() + value.size();
            auto const [ptr, error] = std::from_chars(value.data(), end, parsed);
            if (error == std::errc{} && ptr == end)
            {
                return parsed;
            }
        }
    }
    return tDefault;
}

template<typename T, std::size_t N>
auto ReadArray(ryml::ConstNodeRef tNode, std::array<T, N> tDefault = {}) -> std::array<T, N>
{
    if (!tNode.readable() || !tNode.is_seq() || tNode.num_children() != N)
    {
        return tDefault;
    }

    std::size_t index{};
    for (auto child : tNode.children())
    {
        tDefault[index] = Read(child, tDefault[index]);
        ++index;
    }
    return tDefault;
}

}// namespace polos::utils::yaml

#endif// POLOS_UTILS_YAML_READ_HPP
