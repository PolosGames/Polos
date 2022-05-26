#include "polos/polos_pch.h"

#include "renderer.h"

namespace polos
{
    Renderer* Renderer::m_Instance;

    void Renderer::Startup()
    {
        m_Instance = this;
    }
    
    void Renderer::Shutdown()
    {
        m_Instance = nullptr;
    }
}