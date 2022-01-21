#pragma once

#include "core.h"
#include "application.h"

#ifdef PL_PLATFORM_WINDOWS

extern polos::application *polos::create_application();

int main(int argc, char **argv)
{
	PL_CORE_ERROR("ERR");
	polos::application *app = polos::create_application();
	app->run();
	delete app;
}

#endif