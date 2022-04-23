//
// Created by Kayra Urfalı on 18.03.2022.
//

#ifndef POLOS_CONCEPTS_H
#define POLOS_CONCEPTS_H

#include <type_traits>

namespace polos
{
    template<typename D, typename B>
    concept Derived = std::is_base_of_v<B, D>;
    
    template<typename T>
    concept IsClass = std::is_class_v<T>;
    
    template<typename T, typename U>
    concept IsSame = std::is_same_v<T, U>;
    
    template<typename T, typename U>
    concept IsNotSame = !IsSame<T, U>;
    
    template<typename T>
    concept IsDefaultConstructible = std::is_default_constructible_v<T>;
    
    template<typename T>
    concept IsCopyConstructible = std::is_copy_constructible_v<T>;
    template<typename T>
    concept IsCopyAssignable    = std::is_copy_assignable_v<T>;
    
    template<typename T>
    concept IsMoveConstructible = std::is_move_constructible_v<T>;
    template<typename T>
    concept IsMoveAssignable    = std::is_move_assignable_v<T>;

    template<typename T>
    concept IsTriviallyDestructible  = std::is_trivially_destructible_v<T>;

    template<typename Func>
    concept IsMemFunPtr         = requires
    {
        std::is_member_function_pointer_v<Func>;
    };

    template<typename Func>
    concept IsNotMemFunPtr         = requires
    {
        !std::is_member_function_pointer_v<Func>;
    };
}

#endif //POLOS_CONCEPTS_H
