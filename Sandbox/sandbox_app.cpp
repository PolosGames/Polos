#include <polos.h>

class SandboxApp : public polos::Application
{
public:
	SandboxApp() {}
	~SandboxApp() {}
};

polos::Application* polos::CreateApplication(void* ptr)
{
	if (ptr != nullptr)
	{
		return new (ptr) SandboxApp();
	}
	else
	{
		return new SandboxApp();
	}
}