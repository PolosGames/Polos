#include <polos.h>

class SandboxApp : public polos::Application
{
public:
	SandboxApp() {}
	~SandboxApp() {}
};

polos::Application *polos::create_application()
{
	return new SandboxApp();
}