#include "polos/communication/engine_terminate.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/window_close.hpp"
#include "polos/core/main_loop.hpp"

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
            MainLoop loop;
            communication::Subscribe<communication::EngineUpdate>([](communication::EngineUpdate&) {
                std::exit(EXIT_FAILURE);
            });

            communication::DispatchNow<communication::WindowClose>(nullptr);
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
            MainLoop loop;
            communication::Subscribe<communication::EngineUpdate>([](communication::EngineUpdate&) {
                std::exit(EXIT_FAILURE);
            });

            communication::DispatchNow<communication::EngineTerminate>();
            loop.Run();

            std::exit(EXIT_SUCCESS);
        },
        ::testing::ExitedWithCode(EXIT_SUCCESS),
        "");
}

}// namespace
}// namespace polos::core
