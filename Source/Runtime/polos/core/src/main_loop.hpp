///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_CORE_SRC_MAIN_LOOP_HPP
#define POLOS_CORE_SRC_MAIN_LOOP_HPP

#include "polos/polos_api.hpp"

namespace polos::rendering
{
class IRenderSystem;
}// namespace polos::rendering

namespace polos::communication
{
struct EngineUpdate;
struct WindowClose;
}// namespace polos::communication

namespace polos::core
{
class MainLoop
{
public:
    MainLoop();

    void Run() const;
private:
    void on_window_close();
    void on_engine_terminate();

    bool mIsRunning{true};
};
}// namespace polos::core

#endif// POLOS_CORE_SRC_MAIN_LOOP_HPP
