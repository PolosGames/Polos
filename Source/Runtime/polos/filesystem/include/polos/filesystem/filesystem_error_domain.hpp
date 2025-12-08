///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP
#define POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP

#include "polos/communication/error_code.hpp"
#include "polos/communication/error_domain.hpp"

#include <array>

namespace polos::fs
{

enum class FilesystemErrc : communication::ErrorDomain::CodeType
{
    kFileWasNotFound,
    kFileOpenError,

    kFilesystemErrcCount,
};

class FilesystemErrorDomain : public communication::ErrorDomain
{
public:
    using Errc = FilesystemErrc;

    constexpr FilesystemErrorDomain()
        : polos::communication::ErrorDomain(polos::communication::kFilesystemErrorDomainId),
          m_messages({
              "Could not open file.",
              "Error when opening the file!",
          })
    {}

    virtual ~FilesystemErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    {
        return "Filesystem";
    }

    [[nodiscard]] constexpr auto Message(CodeType t_code) const -> std::string_view override
    {
        return m_messages[static_cast<std::size_t>(t_code)];
    }
private:
    std::array<std::string_view const, static_cast<std::size_t>(FilesystemErrc::kFilesystemErrcCount)> const m_messages;
};

namespace internal
{

constexpr FilesystemErrorDomain g_error_domain;

}// namespace internal

constexpr auto MakeErrorCode(FilesystemErrorDomain::Errc t_err) -> communication::ErrorCode
{
    return communication::ErrorCode{static_cast<communication::ErrorDomain::CodeType>(t_err), internal::g_error_domain};
}

}// namespace polos::fs

#endif// POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP
