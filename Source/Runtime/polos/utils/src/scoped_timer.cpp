///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/scoped_timer.hpp"

#include "polos/logging/log_macros.hpp"

#include <string_view>

namespace polos::utils
{

ScopedTimer::ScopedTimer(std::string t_name)
    : m_name{t_name},
      m_start{GetTimeNow()}
{
    LogInfo("Starting timer for scope: \"{}\"", m_name);
}

ScopedTimer::~ScopedTimer()
{
    auto const passed_time = GetTimeNow() - m_start;

    LogInfo("Scope: \"{}\", took: {}s", m_name, (static_cast<float>(passed_time.count()) * 0.001f * 0.001f));
}

}// namespace polos::utils
