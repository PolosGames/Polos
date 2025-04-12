//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef CORE_ENGINE_ENGINE_SUBSYSTEM_H
#define CORE_ENGINE_ENGINE_SUBSYSTEM_H

#include "polos/datatypes/singleton.hpp"



namespace polos::system
{
template<typename S>
class EngineSubsystem
{
public:
    EngineSubsystem();
    virtual ~EngineSubsystem() = default;
};
} // namespace polos::core

#endif //CORE_ENGINE_ENGINE_SUBSYSTEM_H
