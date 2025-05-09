//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/utils/time/scoped_timer.hpp"

#include "polos/logging/log_macros.hpp"

#include <string_view>

namespace polos::utils
{

class ScopedTimer::Impl
{
public:
    explicit Impl(std::string_view const t_name)
        : m_name{t_name},
          m_start{GetTimeNow()},
          m_end{GetTimeNow()}
    {
        LogInfo("Starting timer for scope: \"{}\"", m_name);
    }

    ~Impl()
    {
        m_end = GetTimeNow();
        auto const passed_time = m_end - m_start;

        LogInfo("Scope: \"{}\", took: {}s", m_name, (static_cast<double>(passed_time) * 0.001f * 0.001f));
    }
private:
    std::string m_name;
    std::uint64_t m_start;
    std::uint64_t m_end;
};

ScopedTimer::ScopedTimer(char const* t_name)
{
    m_impl = new Impl(t_name);
}

ScopedTimer::~ScopedTimer()
{
    delete m_impl;
}

}// namespace polos::utils
