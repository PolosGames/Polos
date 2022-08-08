#ifndef POLOS_CONCEPTS_H
#define POLOS_CONCEPTS_H

#include <type_traits>

namespace polos
{
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
    concept IsTriviallyDestructible = std::is_trivially_destructible_v<T>;

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
