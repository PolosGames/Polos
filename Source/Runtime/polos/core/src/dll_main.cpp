//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#define NOMINMAX
#include <windows.h>

#include "polos/logging/logger.hpp"

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            // Code to run when the DLL is loaded
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            // Code to run when a thread is created or destroyed
            break;
        case DLL_PROCESS_DETACH:
            // Ensure any pending logs are flushed before the DLL is unloaded
            polos::logging::FlushLogger(polos::logging::Logger::Instance().GetAppLogger());
            polos::logging::FlushLogger(polos::logging::Logger::Instance().GetPollyLogger());
            polos::logging::FlushLogger(polos::logging::Logger::Instance().GetPolosLogger());
            break;
    }
    return TRUE;// Successfully processed
}
