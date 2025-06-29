///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_
#define POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_

#include "polos/platform/module_macros.hpp"

#include <GLFW/glfw3.h>

#include <vector>

namespace polos::platform
{

class PLATFORM_EXPORT WindowManager
{
public:
    ~WindowManager();

    WindowManager(WindowManager const&) = delete;
    WindowManager(WindowManager&&) = delete;

    WindowManager& operator=(WindowManager const&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    static WindowManager& Instance();


private:
    WindowManager();

    class Impl;
    Impl* m_impl;
};

}// namespace polos::platform

#endif// POLOS_PLATFORM_INCLUDE_POLOS_PLATFORM_WINDOW_MANAGER_HPP_