#pragma once

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
