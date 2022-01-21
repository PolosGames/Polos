#include <polos.h>
#include <core/entry_point.h>

class senaz_app : public polos::application
{
public:
	senaz_app() {}
	~senaz_app() {}
};

polos::application *polos::create_application()
{
	return new senaz_app();
}