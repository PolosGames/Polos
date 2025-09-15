//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_CODE_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_CODE_HPP_

#include "polos/communication/error_domain.hpp"

#include <expected>
#include <string_view>
#include <type_traits>

namespace polos::communication
{

template<typename T>
concept HasMakeErrorCode = requires { std::is_same_v<T, decltype(MakeErrorCode(std::declval<T>()))>; };

///
/// @brief A wrapper for Enum-typed error codes
///
class ErrorCode
{
public:
    using CodeType = typename ErrorDomain::CodeType;

    ///
    /// @brief Constructs a ErrorCode with the given raw error code.
    /// If MakeError code is not defined inside the error domain file, this constructor is not used in overload resolution
    ///
    /// @tparam EnumT A raw enum-type value that contains the error code value
    ///         Requires MakeErrorCode to be present
    /// @param t_code An enum-typed error code that belongs to a specific domain.
    template<typename EnumT>
        requires(HasMakeErrorCode<EnumT> && !std::is_same_v<EnumT, ErrorCode>)
    constexpr ErrorCode(EnumT t_code)
        : ErrorCode(MakeErrorCode(t_code))
    {}

    constexpr explicit ErrorCode(CodeType t_code, ErrorDomain const& t_domain)
        : m_code{t_code},
          m_domain{t_domain}
    {}

    ///
    /// @brief Return the raw error code enum.
    /// @return Raw error code value
    ///
    constexpr CodeType Code() const
    {
        return m_code;
    }

    ///
    /// @brief Return the domain that this ErrorCode object belongs to.
    /// @return ErrorDomain.
    ///
    constexpr ErrorDomain const& Domain() const
    {
        return m_domain.get();
    }

    ///
    /// @brief Return the message for this error code, that is determined by the ErrorDomain
    /// @return Error message asssociated with this error code.
    ///
    constexpr std::string_view Message() const
    {
        return Domain().Message(Code());
    }

    ///
    /// @brief Return the message for this error code, that is determined by the ErrorDomain
    /// @return Error message asssociated with this error code.
    ///
    constexpr operator std::string_view() const
    {
        return Message();
    }
private:
    CodeType                                  m_code;
    std::reference_wrapper<ErrorDomain const> m_domain;
};

constexpr inline bool operator==(ErrorCode const& t_lhs, ErrorCode const& t_rhs)
{
    return (t_lhs.Domain() == t_rhs.Domain()) && (t_lhs.Code() && t_rhs.Code());
}

constexpr inline bool operator!=(ErrorCode const& t_lhs, ErrorCode const& t_rhs)
{
    return !operator==(t_lhs, t_rhs);
}

}// namespace polos::communication

namespace polos
{

/// @brief Helper alias for return values of functions that return ErrorCode
template<typename ExpectedT>
using Result = std::expected<ExpectedT, communication::ErrorCode>;

/// @brief Helper alias for returning std::unexpected.
using ErrorType = std::unexpected<communication::ErrorCode>;

}// namespace polos

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_CODE_HPP_
