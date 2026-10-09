///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/ecs/entity.hpp"

#include "polos/ecs/settings.hpp"

namespace polos::ecs
{
auto IsEntityValid(EntityHandle tHandle) -> bool
{ return tHandle.index < kInvalidEntityIndex && tHandle.version < kInvalidEntityVersion; }
}// namespace polos::ecs
