//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/polos_config.hpp"

#if defined(POLOS_WIN)
#    define NOMINMAX
#    include <windows.h>

#    include "polos/logging/log_macros.hpp"
#    include "polos/logging/logger.hpp"

BOOL APIENTRY DllMain(HMODULE /*h_module*/, DWORD ul_reason_for_call, LPVOID /*reserved*/)
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
            LogCriticalCtx(LOG_CTX_POLOS, "Unloading polos_rendering.dll");
            // Ensure any pending logs are flushed before the DLL is unloaded
            polos::logging::FlushLogger(LOG_CTX_APP);
            polos::logging::FlushLogger(LOG_CTX_POLLY);
            polos::logging::FlushLogger(LOG_CTX_POLOS);
            break;
    }
    return TRUE;
}
#endif// POLOS_WIN
