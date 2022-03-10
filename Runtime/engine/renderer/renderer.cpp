#include "plpch.h"
#include "renderer.h"


namespace polos
{
	void Renderer::Startup()
	{
		m_Instance = this;
	}
	
	void Renderer::Shutdown()
	{
		m_Instance = nullptr;
	}
}