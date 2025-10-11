///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_INCLUDE_POLOS_CORE_MAIN_LOOP_HPP_
#define POLOS_CORE_INCLUDE_POLOS_CORE_MAIN_LOOP_HPP_

#include "polos/core/module_macros.hpp"

namespace polos::communication
{
struct engine_update;
struct window_close;
}// namespace polos::communication

namespace polos::core
{
class CORE_EXPORT MainLoop
{
public:
    MainLoop();

    void Run();
private:
    void on_window_close();
    void on_engine_terminate();

    bool m_is_running{true};
};
}// namespace polos::core

#endif// POLOS_CORE_INCLUDE_POLOS_CORE_MAIN_LOOP_HPP_
