#pragma once

#include <typeinfo>

#include "stringid.h"
#include "polos/containers/containers.h"

namespace polos
{
    template<typename T>
    StringId TypeHash()
    {
        std::string const name = typeid(T).name();
        StringId const    id   = GetStringId(name);

        return id;
    }

    template<typename T>
        requires std::is_arithmetic_v<T>
    std::array<byte, sizeof(T)> ToByteArray(T p_Value)
    {
        std::size_t                     i{sizeof(p_Value) * 8};
        std::array<byte, sizeof(p_Value)> byte_array;
        while (i != 0)
        {
            byte_array[(i / 8) - 1] = static_cast<byte>(p_Value >> i);
            i -= 8;
        }

        return byte_array;
    }

    template <typename Test, template <typename...> class Ref>
    struct is_specialization : std::false_type {};

    template <template <typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    template <typename Test, template <typename...> class Ref>
    static inline constexpr bool is_specialization_v = is_specialization<Test, Ref>::value;
}
