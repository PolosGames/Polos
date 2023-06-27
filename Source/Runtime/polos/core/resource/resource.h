#pragma once

#define INVALID_RESOURCE 18446744073709551615

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
