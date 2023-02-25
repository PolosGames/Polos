#pragma once

namespace polos
{
    class Gui
    {
    public:
        static void Setup();
        static void Shutdown();
        
        static void Begin();
        static void End();

        static void* GetContext();

    private:
        static void* s_Context;
    };
}
