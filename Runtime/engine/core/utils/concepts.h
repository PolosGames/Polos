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
}

#endif //POLOS_CONCEPTS_H
