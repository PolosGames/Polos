///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_UTILS_ERROR_DOMAIN_HPP
#define POLOS_UTILS_UTILS_ERROR_DOMAIN_HPP

#include "polos/utils/error_code.hpp"
#include "polos/utils/error_domain.hpp"

namespace polos::utils
{
// TODO(sorbatdev): Auto generate error codes and messages from a JSON or XML file
enum class UtilsErrc : utils::ErrorDomain::CodeType
{
    kValueNotFoundInMap,

    kUtilsErrcCount,
};

class UtilsErrorDomain : public utils::ErrorDomain
{
public:
    using Errc = UtilsErrc;

    constexpr UtilsErrorDomain()
        : polos::utils::ErrorDomain(polos::utils::kUtilsErrorDomainId),
          mMessages({"Cannot find value in Constexpr Map!"})
    {}

    virtual ~UtilsErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    { return "Utils"; }

    [[nodiscard]] constexpr auto Message(CodeType tCode) const -> std::string_view override
    { return mMessages[static_cast<std::size_t>(tCode)]; }
private:
    std::array<std::string_view const, static_cast<std::size_t>(UtilsErrc::kUtilsErrcCount)> const mMessages;
};

namespace internal
{

constexpr UtilsErrorDomain g_error_domain;

}// namespace internal

constexpr utils::ErrorCode MakeErrorCode(UtilsErrorDomain::Errc tErr)
{ return utils::ErrorCode{static_cast<utils::ErrorDomain::CodeType>(tErr), internal::g_error_domain}; }
}// namespace polos::utils

#endif// POLOS_UTILS_UTILS_ERROR_DOMAIN_HPP
