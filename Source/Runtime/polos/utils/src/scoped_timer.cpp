///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/utils/scoped_timer.hpp"

#include "polos/logging/log_macros.hpp"

#include <string_view>

namespace polos::utils
{

class ScopedTimer::Impl
{
public:
    explicit Impl(std::string_view const t_name)
        : m_name{t_name},
          m_start{GetTimeNow()}
    {
        LogInfo("Starting timer for scope: \"{}\"", m_name);
    }

    ~Impl()
    {
        auto const passed_time = GetTimeNow() - m_start;

        LogInfo("Scope: \"{}\", took: {}s", m_name, (static_cast<float>(passed_time.count()) * 0.001f * 0.001f));
    }
private:
    std::string m_name;
    TimePoint   m_start;
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
