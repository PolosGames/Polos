#pragma once

#ifdef PL_PLATFORM_WINDOWS
  #ifdef PL_BUILD_DLL
    #define POLOSAPI __declspec(dllexport)
  #else
    #define POLOSAPI __declspec(dllimport)
  #endif
#else
  #error Only Windows is supported
#endif