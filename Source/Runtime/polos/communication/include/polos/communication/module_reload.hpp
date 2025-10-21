//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_MODULE_RELOAD_HPP_
#define POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_MODULE_RELOAD_HPP_

#include "polos/communication/event.hpp"
#include "polos/rendering/shared_lib_out.hpp"

namespace polos::communication
{

struct module_reload : base_event
{
    DECLARE_POLOS_EVENT(module_reload);

    explicit module_reload(std::string_view const t_module_name, rendering::rendering_shared_lib_out& t_module)
        : module_name{t_module_name},
          module{t_module}
    {}

    std::string_view                     module_name;
    rendering::rendering_shared_lib_out& module;
};

}// namespace polos::communication

DEFINE_EVENT_LOG_FORMAT(polos::communication::module_reload, "Reloaded module: {}", event.module_name);

#endif// POLOS_COMMUNICATION_INCLUDE_POLOS_COMMUNICATION_MODULE_RELOAD_HPP_
