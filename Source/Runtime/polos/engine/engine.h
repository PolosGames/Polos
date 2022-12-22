#pragma once

#ifndef POLOS_CORE_ENGINE_H_
#define POLOS_CORE_ENGINE_H_

#include <stack>

namespace polos
{
    class Engine
    {
        using Subscriber = void(*)(void);
    public:
        static void Run();

        static Engine& Instance()
        {
            static Engine e;
            return e;
        }
    };
} // namespace polos

#endif /* POLOS_CORE_ENGINE_H_ */