//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_DOMAIN_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_DOMAIN_HPP_

#include <cstdint>
#include <string_view>

namespace polos::communication
{

///
/// @brief A class representing a domain's all error values and their related messages.
/// This class also controls creation of error-codes that are related to a domain's (Engine component) error enums.
/// This class also must be inherited for each domain that is intended to be created. Each domain has a pre-defined
/// domain-id which should be consumed when initializing this base class.
class ErrorDomain
{
public:
    /// @brief Domain specific id type.
    using IdType = std::uint8_t;

    /// @brief Underlying type for all error codes belonging to a domain
    using CodeType = std::uint8_t;

    ErrorDomain(ErrorDomain const&) = delete;
    ErrorDomain(ErrorDomain&&)      = delete;

    ErrorDomain& operator=(ErrorDomain const&) = delete;
    ErrorDomain& operator=(ErrorDomain&&)      = delete;

    ///
    /// @brief Returns the name of the error domain
    /// @return Name of error domain in a null-terminated string.
    ///
    virtual std::string_view Name() const = 0;

    ///
    /// @brief Determines and returns the message that is corresponding to the error-code given.
    /// @return Name of error domain in a null-terminated string.
    ///
    virtual std::string_view Message(CodeType t_code) const = 0;

    ///
    /// @brief Returns the pre-determined id for this domain
    /// @return Id for this domain
    ///
    constexpr IdType Id() const
    {
        return m_id;
    }
protected:
    constexpr explicit ErrorDomain(IdType t_id)
        : m_id{t_id}
    {}

    ~ErrorDomain() = default;
private:
    IdType const m_id;
};

constexpr bool operator==(ErrorDomain const& t_lhs, ErrorDomain const& t_rhs)
{
    return t_lhs.Id() == t_rhs.Id();
}

constexpr bool operator!=(ErrorDomain const& t_lhs, ErrorDomain const& t_rhs)
{
    return !operator==(t_lhs, t_rhs);
}

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_ERROR_DOMAIN_HPP_
