#ifndef POLOS_CONCEPTS_H
#define POLOS_CONCEPTS_H

#include <type_traits>

namespace polos
{
    template<typename T>
    concept DefaultConstructible = std::is_default_constructible_v<T>;
    
    template<typename T>
    concept CopyConstructible = std::is_copy_constructible_v<T>;
    template<typename T>
    concept CopyAssignable = std::is_copy_assignable_v<T>;
    
    template<typename T>
    concept MoveConstructible = std::is_move_constructible_v<T>;
    template<typename T>
    concept MoveAssignable = std::is_move_assignable_v<T>;

    template<typename T>
    concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

    template<typename Func>
    concept MemberFunctionPtr         = requires
    {
        std::is_member_function_pointer_v<Func>;
    };

    template<typename Func>
    concept NonMemberFunctionPtr      = requires
    {
        !std::is_member_function_pointer_v<Func>;
    };
}

#endif //POLOS_CONCEPTS_H
