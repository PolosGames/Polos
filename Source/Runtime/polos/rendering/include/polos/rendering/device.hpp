//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_

#include "polos/communication/system_created.hpp"
#include "polos/rendering/module_macros.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{

class RENDERING_EXPORT Device
{
public:
    Device();
    ~Device();

    Device(Device&&)            = delete;
    Device(Device&)             = delete;
    Device& operator=(Device&&) = delete;
    Device& operator=(Device&)  = delete;
private:
    void on_system_created(communication::system_created& t_event);
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_DEVICE_HPP_
