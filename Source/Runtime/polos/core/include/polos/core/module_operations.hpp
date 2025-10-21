//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_CORE_INCLUDE_POLOS_CORE_MODULE_OPERATIONS_HPP_
#define POLOS_CORE_INCLUDE_POLOS_CORE_MODULE_OPERATIONS_HPP_

#include "polos/core/module_macros.hpp"
#include "polos/utils/hot_reload_utils.hpp"

namespace polos::core
{

[[nodiscard]] CORE_EXPORT auto LoadAllModules() -> bool;
CORE_EXPORT auto               UnloadAllModules() -> void;

[[nodiscard]] CORE_EXPORT auto LoadRenderingModule() -> bool;
[[nodiscard]] CORE_EXPORT auto GetRenderingModule() -> utils::base_shared_lib_out&;

}// namespace polos::core

#endif// POLOS_CORE_INCLUDE_POLOS_CORE_MODULE_OPERATIONS_HPP_
