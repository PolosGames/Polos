#include "polos/communication/event_bus.hpp"
#include "polos/core/events/engine_terminate.hpp"
#include "polos/core/events/engine_update.hpp"
#include "polos/core/main_loop.hpp"
#include "polos/ecs/system_registry.hpp"
#include "polos/platform/events/window_close.hpp"

#include <gtest/gtest.h>

#include <cstdlib>

namespace polos::core
{
namespace
{

// The event bus retains callbacks; each child process owns one complete loop lifetime.
TEST(MainLoopTest, WindowCloseStopsBeforeFirstFrame)
{
    EXPECT_EXIT(
        {
            ecs::SystemRegistry systems;
            MainLoop            loop;
            communication::Subscribe<EngineUpdate>([](EngineUpdate&) {
                std::exit(EXIT_FAILURE);
            });

            communication::DispatchNow<platform::WindowClose>(nullptr);
            loop.Run();

            std::exit(EXIT_SUCCESS);
        },
        ::testing::ExitedWithCode(EXIT_SUCCESS),
        "");
}

TEST(MainLoopTest, EngineTerminateStopsBeforeFirstFrame)
{
    EXPECT_EXIT(
        {
            ecs::SystemRegistry systems;
            MainLoop            loop;
            communication::Subscribe<EngineUpdate>([](EngineUpdate&) {
                std::exit(EXIT_FAILURE);
            });

            communication::DispatchNow<EngineTerminate>();
            loop.Run();

            std::exit(EXIT_SUCCESS);
        },
        ::testing::ExitedWithCode(EXIT_SUCCESS),
        "");
}

}// namespace
}// namespace polos::core
