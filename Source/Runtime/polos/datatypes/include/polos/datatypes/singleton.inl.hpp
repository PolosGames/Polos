///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_DATATYPES_INCLUDE_POLOS_DATATYPES_SINGLETON_INL_HPP_
#define POLOS_DATATYPES_INCLUDE_POLOS_DATATYPES_SINGLETON_INL_HPP_

#include "polos/datatypes/singleton.hpp"
#include "polos/logging/log_macros.hpp"

#include <type_traits>

namespace polos::datatypes
{

template<typename Type>
bool Singleton<Type>::s_is_constructed{false};

template<typename Type>
template<typename... Args>
Type* Singleton<Type>::Instance(Args&&... t_args)
{
    static data s_data{};
    if (!s_is_constructed)
    {
        new (&s_data.instance) Type(std::forward<Args>(t_args)...);
        s_is_constructed = true;
    }
    return &s_data.instance;
}

template<typename Type>
bool Singleton<Type>::Destroy()
{
    if (!s_is_constructed)
    {
        return false;
    }

    s_is_constructed = false;

    LOG_ENGINE_INFO("Destroying subsystem...");

    Instance()->~Type();

    return true;
}


}// namespace polos::datatypes

#endif// POLOS_DATATYPES_INCLUDE_POLOS_DATATYPES_SINGLETON_INL_HPP_
