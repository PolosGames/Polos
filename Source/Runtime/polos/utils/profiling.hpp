///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_UTILS_PROFILING_HPP
#define POLOS_UTILS_PROFILING_HPP

#include "polos/polos_config.hpp"

#if defined(ENABLE_PROFILING)
#    include <tracy/Tracy.hpp>

#    define POLOS_PROFILE_ZONE(Name)        ZoneNamedN(TracyConcat(polosCpuZone, __LINE__), Name, true)
#    define POLOS_PROFILE_FRAME()           FrameMark
#    define POLOS_PROFILE_PLOT(Name, Value) TracyPlot(Name, Value)
#else
#    define POLOS_PROFILE_ZONE(Name)        static_cast<void>(0)
#    define POLOS_PROFILE_FRAME()           static_cast<void>(0)
#    define POLOS_PROFILE_PLOT(Name, Value) static_cast<void>(0)
#endif

#endif// POLOS_UTILS_PROFILING_HPP
