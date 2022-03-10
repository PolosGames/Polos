#include "plpch.h"

#include "time/timer.h"
#include "memory/linear_allocator.h"

#include "log.h"
#include "event_bus.h"
#include "renderer/renderer.h"
#include "application.h"

#include "events/engine/engine_stop.h"

#include "engine.h"


namespace polos
{
	void Engine::Run()
	{
		time::Timer::OnStartUp();

		// Allocate enough memory for the whole engine.
		// Zeroes are so that git
		size_t needed_memory =
			sizeof(Log)         +
			sizeof(EventBus)    +
			sizeof(Application) +
			0;
			
		memory::LinearAllocator engine_memory(needed_memory);

		/// Allocate memory for the subsystems
		Log*      log       = engine_memory.New<Log>();
		EventBus* event_bus = engine_memory.New<EventBus>();
		Renderer* renderer  = engine_memory.New<Renderer>();

		/// Startup for subsystems
		log      ->Startup();
		event_bus->Startup();
		renderer ->Startup();

		Application* app = CreateApplication(nullptr);
		app->Run();

		/// Shutdown sequence
		renderer ->Shutdown();
		event_bus->Shutdown();
		log      ->Shutdown();

		engine_memory.Clear();

		delete app;
	}
} // namespace polos
