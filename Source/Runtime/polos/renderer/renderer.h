#pragma once

namespace polos
{
    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void Startup();
        void Shutdown();
    private:
        static Renderer* s_Instance;
    };
}
