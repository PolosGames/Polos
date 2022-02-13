#include <polos.h>

class senaz_app : public polos::Application
{
public:
	senaz_app() {}
	~senaz_app() {}
};

polos::Application *polos::create_application()
{
	return new senaz_app();
}