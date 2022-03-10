#include <polos.h>

#ifndef SANDBOX_APP_H_
#define SANDBOX_APP_H_

class SandboxApp : public polos::Application
{
public:
	SandboxApp()
	{}
	~SandboxApp()
	{}
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

#endif /* SANDBOX_APP_H_ */