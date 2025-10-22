//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDERING_MODULE_RELOAD_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDERING_MODULE_RELOAD_HPP_

#include "polos/communication/event.hpp"
#include "polos/rendering/shared_lib_out.hpp"

namespace polos::communication
{

struct rendering_module_reload final : base_event
{
    DECLARE_POLOS_EVENT(rendering_module_reload);
};

}// namespace polos::communication

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_RENDERING_MODULE_RELOAD_HPP_
