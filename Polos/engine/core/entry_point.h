#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "utils/platform_defines.h"
#include "application.h"
#include "utils/logger.h"

#ifdef PL_PLATFORM_WINDOWS

extern polos::application *polos::create_application();

int main(int argc, char **argv)
{
	polos::application *app = polos::create_application();
	app->run();
	delete app;
}

#endif

#endif /* POLOS_CORE_ENTRYPOINT_H */
