///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP
#define POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP

#include "polos/utils/error_code.hpp"
#include "polos/utils/error_domain.hpp"

#include <array>

namespace polos::fs
{

enum class FilesystemErrc : utils::ErrorDomain::CodeType
{
    kFileWasNotFound,
    kFileOpenError,

    kFilesystemErrcCount,
};

class FilesystemErrorDomain : public utils::ErrorDomain
{
public:
    using Errc = FilesystemErrc;

    constexpr FilesystemErrorDomain()
        : polos::utils::ErrorDomain(polos::utils::kFilesystemErrorDomainId),
          mMessages({
              "Could not open file.",
              "Error when opening the file!",
          })
    {}

    virtual ~FilesystemErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    { return "Filesystem"; }

    [[nodiscard]] constexpr auto Message(CodeType tCode) const -> std::string_view override
    { return mMessages[static_cast<std::size_t>(tCode)]; }
private:
    std::array<std::string_view const, static_cast<std::size_t>(FilesystemErrc::kFilesystemErrcCount)> const mMessages;
};

namespace internal
{

constexpr FilesystemErrorDomain g_error_domain;

}// namespace internal

constexpr auto MakeErrorCode(FilesystemErrorDomain::Errc tErr) -> utils::ErrorCode
{ return utils::ErrorCode{static_cast<utils::ErrorDomain::CodeType>(tErr), internal::g_error_domain}; }

}// namespace polos::fs

#endif// POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP
