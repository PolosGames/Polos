//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_HOT_RELOAD_UTILS_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_HOT_RELOAD_UTILS_HPP_

#include "polos/polos_config.hpp"

#if defined(POLOS_LINUX)
#    include "polos/utils/linux_hot_reload_utils.hpp"
#elif defined(POLOS_WIN)
#    include "polos/utils/windows_hot_reload_utils.hpp"
#endif

#endif// POLOS_UTILS_INCLUDE_POLOS_UTILS_HOT_RELOAD_UTILS_HPP_
