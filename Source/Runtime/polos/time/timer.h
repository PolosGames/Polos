#pragma once

#include <ratio>

#include "polos/utils/alias.h"
#include "polos/utils/macro_util.h"

namespace polos::time
{
    class Timer
    {
    private:
        using Period = std::micro;
    private:
        Timer();
    public:
        static void  Startup();
        static int64 Now();
    private:
        static int64 s_Freq;
    };
} // namespace polos::core::time
