#pragma once
#ifndef POLOS_RENDERER_RENDERER_H
#define POLOS_RENDERER_RENDERER_H

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

#endif /* POLOS_RENDERER_RENDERER_H */
