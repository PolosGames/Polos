#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "platform/platform_detection.h"
#include "application.h"
#include "logger.h"

#ifdef POLOS_WIN

extern polos::Application *polos::create_application();

int main(int argc, char **argv)
{
	polos::Application *app = polos::create_application();
	app->run();
	delete app;
}

#endif

#endif /* POLOS_CORE_ENTRYPOINT_H */
