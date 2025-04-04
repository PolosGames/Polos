//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_APP_APPLICATION_H
#define CORE_APP_APPLICATION_H

#include "polos/core/module_macros.hpp"

namespace polos::core
{
class CORE_EXPORT Application
{
public:
    Application();

    void Run();

    bool m_is_running;
};
} // namespace polos::core

#endif //CORE_APP_APPLICATION_H
