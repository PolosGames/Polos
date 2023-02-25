#include "polos/polos_pch.h"

#include "renderer.h"

namespace polos
{
    Renderer* Renderer::s_Instance;

    void Renderer::Startup()
    {
        s_Instance = this;
    }
    
    void Renderer::Shutdown()
    {
        s_Instance = nullptr;
    }
}