///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef SANDBOX_HELPERS_HPP
#define SANDBOX_HELPERS_HPP

#include <polos/ecs/entity.hpp>
#include <polos/utils/string_id.hpp>

namespace polos::ecs
{
class Scene;
}// namespace polos::ecs

namespace sandbox
{
auto CreateEntityFromModel(polos::utils::string_id tModel, polos::ecs::Scene* tScene) -> polos::ecs::EntityHandle;

}// namespace sandbox

#endif// SANDBOX_HELPERS_HPP
