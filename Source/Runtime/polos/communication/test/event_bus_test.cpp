//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"

#include <gtest/gtest.h>

namespace
{

TEST(EventBusTestFixture, SubscribeToEventTest)
{
    auto& event_bus = polos::communication::EventBus::Instance();

    float                 ref_delta_time{0.0f};
    constexpr float const expected_delta_time{44.0f};

    auto lambda = [&ref_delta_time](polos::communication::engine_update& t_event)
    {
        LogError("{}", t_event);
        ref_delta_time = t_event.delta_time;
    };

    polos::communication::Subscribe<polos::communication::engine_update>(lambda);
    polos::communication::Dispatch<polos::communication::engine_update>(expected_delta_time);

    EXPECT_EQ(ref_delta_time, expected_delta_time);
}

TEST(EventBusTestFixture, DispatchWithoutSubscribers)
{
    auto& event_bus = polos::communication::EventBus::Instance();

    constexpr float const expected_delta_time{44.0f};

    std::size_t total_dispatched =
        polos::communication::Dispatch<polos::communication::engine_update>(expected_delta_time);

    EXPECT_EQ(total_dispatched, 0);
}

}// namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}