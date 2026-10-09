///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RESOURCE_RESOURCE_ERROR_DOMAIN_HPP
#define POLOS_RESOURCE_RESOURCE_ERROR_DOMAIN_HPP

#include "polos/utils/error_code.hpp"
#include "polos/utils/error_domain.hpp"

#include <array>
#include <string_view>

namespace polos::resource
{

// TODO(sorbatdev): Auto generate error codes and messages from a JSON or XML file
enum class ResourceErrc : utils::ErrorDomain::CodeType
{
    kGenericError,
    kFailedLoadModel,
    kPathTooLong,
    kInvalidGltfBufferIndex,
    kInvalidGltfBufferRange,
    kInvalidGltfAccessorIndex,
    kInvalidGltfBufferViewIndex,
    kUnsupportedGltfSparseAccessor,
    kUnsupportedGltfAccessorType,
    kUnsupportedGltfComponentType,
    kInvalidGltfMaterialIndex,

    kResourceErrcCount,
};

class ResourceErrorDomain : public utils::ErrorDomain
{
public:
    using Errc = ResourceErrc;

    constexpr ResourceErrorDomain()
        : polos::utils::ErrorDomain(polos::utils::kResourceErrorDomainId),
          mMessages({
              "An error occurred while loading a resource.",
              "Failed to load model.",
              "Resource path exceeds the loader's supported length.",
              "glTF buffer view references an invalid buffer index.",
              "glTF buffer view extends beyond its buffer's data.",
              "glTF primitive references an invalid accessor index.",
              "glTF accessor references a missing or invalid buffer view.",
              "Sparse glTF accessors are not supported.",
              "glTF accessor type is not supported.",
              "glTF accessor component type is not supported.",
              "glTF primitive references an invalid material index.",
          })
    {}

    virtual ~ResourceErrorDomain() = default;

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    { return "Resource"; }

    [[nodiscard]] constexpr auto Message(CodeType tCode) const -> std::string_view override
    { return mMessages[static_cast<std::size_t>(tCode)]; }
private:
    std::array<std::string_view const, static_cast<std::size_t>(ResourceErrc::kResourceErrcCount)> const mMessages;
};

namespace internal
{

constexpr ResourceErrorDomain g_error_domain;

}// namespace internal

constexpr utils::ErrorCode MakeErrorCode(ResourceErrorDomain::Errc tErr)
{ return utils::ErrorCode{static_cast<utils::ErrorDomain::CodeType>(tErr), internal::g_error_domain}; }

}// namespace polos::resource

#endif// POLOS_RESOURCE_RESOURCE_ERROR_DOMAIN_HPP
