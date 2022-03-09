#include "plpch.h"

#include "time/timer.h"
#include "memory/linear_allocator.h"
#include "log.h"
#include "event_bus.h"

#include "events/engine/engine_stop.h"
#include "application.h"

#include "engine.h"


namespace polos
{
	void Engine::Run()
	{
		time::Timer::OnStartUp();

		size_t needed_memory =
			sizeof(Log)      +
			sizeof(EventBus) + 
			512;

		memory::LinearAllocator engine_memory(needed_memory);

		Log* log = engine_memory.New<Log>();
		log->StartUp();

		EventBus* event_bus = engine_memory.New<EventBus>();
		event_bus->StartUp();

		polos::Application* app = polos::create_application();
		app->Run();

		event_bus->Shutdown();
		engine_memory.Delete(event_bus);

		engine_memory.Delete(log);

		engine_memory.Clear();
		delete app;
	}
} // namespace polos
