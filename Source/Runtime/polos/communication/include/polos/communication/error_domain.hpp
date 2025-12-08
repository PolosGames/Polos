///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_COMMUNICATION_ERROR_DOMAIN_HPP
#define POLOS_COMMUNICATION_ERROR_DOMAIN_HPP

#include <cstdint>
#include <string_view>

namespace polos::communication
{

constexpr std::uint8_t const kFilesystemErrorDomainId{0x03U};
constexpr std::uint8_t const kRenderingErrorDomainId{0x06U};

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
    [[nodiscard]] virtual auto Name() const -> std::string_view = 0;

    ///
    /// @brief Determines and returns the message that is corresponding to the error-code given.
    /// @return Name of error domain in a null-terminated string.
    ///
    [[nodiscard]] virtual auto Message(CodeType t_code) const -> std::string_view = 0;

    ///
    /// @brief Returns the pre-determined id for this domain
    /// @return Id for this domain
    ///
    [[nodiscard]] constexpr IdType Id() const
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

constexpr auto operator==(ErrorDomain const& t_lhs, ErrorDomain const& t_rhs) -> bool
{
    return t_lhs.Id() == t_rhs.Id();
}

constexpr auto operator!=(ErrorDomain const& t_lhs, ErrorDomain const& t_rhs) -> bool
{
    return !operator==(t_lhs, t_rhs);
}

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_ERROR_DOMAIN_HPP
