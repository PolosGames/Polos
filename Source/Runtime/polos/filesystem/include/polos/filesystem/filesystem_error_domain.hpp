//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP_
#define POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP_

#include "polos/communication/error_code.hpp"
#include "polos/communication/error_domain.hpp"

#include <array>

namespace polos::fs
{

enum class FilesystemErrc : communication::ErrorDomain::CodeType
{
    kFileWasNotFound = 0U,
    kFileOpenError   = 1U,
};

class FilesystemErrorDomain : public communication::ErrorDomain
{
public:
    using Errc = FilesystemErrc;

    constexpr FilesystemErrorDomain()
        : polos::communication::ErrorDomain(0x07U),
          m_messages({
              "Could not open file.",
              "Error when opening the file!",
          })
    {}

    virtual ~FilesystemErrorDomain() = default;

    constexpr std::string_view Name() const override
    {
        return "Filesystem";
    }

    constexpr std::string_view Message(CodeType t_code) const override
    {
        return m_messages[static_cast<std::size_t>(t_code)];
    }
private:
    std::array<std::string_view const, 10> const m_messages;
};

namespace internal
{
inline constexpr FilesystemErrorDomain g_error_domain;
}

constexpr communication::ErrorCode MakeErrorCode(FilesystemErrorDomain::Errc t_err)
{
    return communication::ErrorCode{static_cast<communication::ErrorDomain::CodeType>(t_err), internal::g_error_domain};
}

}// namespace polos::fs

#endif// POLOS_FILESYSTEM_INCLUDE_POLOS_FILESYSTEM_FILESYSTEM_ERROR_DOMAIN_HPP_
