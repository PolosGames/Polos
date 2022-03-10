#pragma once
#ifndef POLOS_CORE_ENTRYPOINT_H
#define POLOS_CORE_ENTRYPOINT_H

#include "platform/platform_detection.h"
#include "application.h"
#include "log.h"
#include "engine/engine.h"

#ifdef POLOS_WIN

extern polos::Application* polos::create_application(void* ptr);

int main(int argc, char **argv)
{
	polos::Engine::Run();
}

#endif

#endif /* POLOS_CORE_ENTRYPOINT_H */
