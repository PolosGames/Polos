#pragma once

#include "polos/core/resource/types/image.h"

#include "polos/core/resource/loaders/image_loader_stb.h"

namespace polos::resource
{
    template<typename T>
    auto LoadResource(std::string p_Path) -> ResourceHandle;

    template<typename T>
    auto GetResource(std::string p_Name) -> ResourceHandle;
    
    template<typename T>
    auto GetRawResource(std::string p_Name) -> T*;

    template<typename T>
    auto DestroyResource(std::string p_Name) -> void;
} // namespace polos::resource

#include "resource.inl"
