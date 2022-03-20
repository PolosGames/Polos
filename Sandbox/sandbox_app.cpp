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
    Application* app = !ptr ? new SandboxApp() : new (ptr) SandboxApp();
    return app;
}

#endif /* SANDBOX_APP_H_ */