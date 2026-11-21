///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_SCENE_SCENE_ERROR_DOMAIN_HPP
#define POLOS_SCENE_SCENE_ERROR_DOMAIN_HPP

#include "polos/utils/error_code.hpp"

#include <array>
#include <cstddef>

namespace polos::scene
{
enum class SceneErrc : utils::ErrorDomain::CodeType
{
    kNoCurrentScene,
    kMissingScenePath,
    kSceneSerializationFailed,
    kSceneFileOpenFailed,
    kSceneFileWriteFailed,
    kSceneFileReplaceFailed,
};

class SceneErrorDomain final : public utils::ErrorDomain
{
public:
    constexpr SceneErrorDomain()
        : utils::ErrorDomain(utils::kSceneErrorDomainId)
    {}

    [[nodiscard]] constexpr auto Name() const -> std::string_view override
    {
        return "Scene";
    }

    [[nodiscard]] constexpr auto Message(CodeType tCode) const -> std::string_view override
    {
        constexpr std::array<std::string_view, 6> messages{
            "There is no current scene to save.",
            "The scene has no registered save path.",
            "Could not serialize the scene to YAML.",
            "Could not open the temporary scene file.",
            "Could not write the scene file.",
            "Could not replace the destination scene file.",
        };
        return messages[static_cast<std::size_t>(tCode)];
    }
};

inline constexpr SceneErrorDomain kSceneErrorDomain;

constexpr auto MakeErrorCode(SceneErrc tError) -> utils::ErrorCode
{
    return utils::ErrorCode{static_cast<utils::ErrorDomain::CodeType>(tError), kSceneErrorDomain};
}
}// namespace polos::scene

#endif// POLOS_SCENE_SCENE_ERROR_DOMAIN_HPP
