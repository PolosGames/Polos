#pragma once

#include "polos/utils/type_util.h"
#include "polos/utils/feature.h"

namespace polos
{
    template <class Err>
    class unexpected;

    template <class _Err>
    struct check_unexpected_argument : std::true_type
    {
        static_assert(std::is_object_v<_Err>, "E must be an object type.");
        static_assert(!std::is_array_v<_Err>, "E must not be an array type.");
        static_assert(!std::is_const_v<_Err>, "E must not be const.");
        static_assert(!std::is_volatile_v<_Err>, "E must not be volatile.");
        static_assert(!is_specialization_v<_Err, unexpected>, "E must not be a specialization of unexpected.");
    };

    template<typename E>
    class unexpected
    {
        static_assert(check_unexpected_argument<E>::value);
    public:
        constexpr unexpected(unexpected const&) = default;
        constexpr unexpected(unexpected&&)      = default;

        constexpr unexpected(E p_Error);
    public:
        PL_NODISCARD constexpr auto error() & noexcept       -> E&;
        PL_NODISCARD constexpr auto error() && noexcept      -> E&&;
        PL_NODISCARD constexpr auto error() const& noexcept  -> E const&;
        PL_NODISCARD constexpr auto error() const&& noexcept -> E const&&;

        constexpr void swap(unexpected& p_Other) noexcept(std::is_nothrow_swappable_v<E>) requires std::is_swappable_v<E>;
    private:
        E m_Error;
    };

    //template<typename T, typename E>
    //class expected
    //{
    //private:
    //    using value_type = T;
    //    using error_type = E;
    //    using unexpected_type = 
    //public:

    //};

    template<typename E1, typename E2>
    PL_NODISCARD constexpr bool operator==(unexpected<E1> const& p_Lhs, unexpected<E2> const& p_Rhs) noexcept
    {
        return p_Lhs.error() == p_Rhs.error();
    }

    template<typename E>
        requires std::is_swappable_v<E>
    constexpr void swap(unexpected<E>& p_Lhs, unexpected<E>& p_Rhs) noexcept(std::is_nothrow_swappable_v<_Err>)
    {
        
    }
} // namespace polos
