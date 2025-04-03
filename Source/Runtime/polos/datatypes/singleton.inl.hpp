//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/datatypes/singleton.hpp"

#ifndef DATATYPES_SINGLETON_INL_H
#define DATATYPES_SINGLETON_INL_H

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

} // namespace polos::datatypes

#endif //DATATYPES_SINGLETON_INL_H
