#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "platform/platform_detection.h"
#include "application.h"
#include "logger.h"
#include "core/time/clock.h"

#ifdef POLOS_WIN

extern polos::Application *polos::create_application();

int main(int argc, char **argv)
{
	polos::time::Clock::Initialize();

	polos::Application *app = polos::create_application();
	app->Run();
	delete app;
}

#endif

#endif /* POLOS_CORE_ENTRYPOINT_H */
