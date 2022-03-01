#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "platform/platform_detection.h"
#include "application.h"
#include "logger.h"
#include "engine/engine.h"

#ifdef POLOS_WIN

extern polos::Application *polos::create_application();

int main(int argc, char **argv)
{
	polos::Engine::Run();

	polos::Application *app = polos::create_application();
	app->Run();
	delete app;

	polos::Engine::Stop();
}

#endif

#endif /* POLOS_CORE_ENTRYPOINT_H */
