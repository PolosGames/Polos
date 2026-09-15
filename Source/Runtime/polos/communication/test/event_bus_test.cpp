///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/logging/log_macros.hpp"

#include <gtest/gtest.h>

namespace
{

class EventBusTestFixture : public ::testing::Test
{
protected:
    static constexpr float const    kExpectedDeltaTime{44.0F};
    polos::communication::EventBus& mEventBus = polos::communication::EventBus::Instance();
};

TEST_F(EventBusTestFixture, SubscribeToEventTest)
{
    float ref_delta_time{0.0F};

    auto lambda = [&ref_delta_time](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda);
    polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(ref_delta_time, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, SubscribeMultipleTimes)
{
    float ref_delta_time_1{0.0F};
    float ref_delta_time_2{0.0F};

    auto lambda_1 = [&ref_delta_time_1](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time_1 = tEvent.deltaTime;
    };

    auto lambda_2 = [&ref_delta_time_2](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time_2 = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda_1);
    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda_2);
    polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(ref_delta_time_1, kExpectedDeltaTime);
    EXPECT_EQ(ref_delta_time_2, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, DispatchWithoutSubscribers)
{
    std::size_t const total_dispatched =
        polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(total_dispatched, 0U);
}

TEST_F(EventBusTestFixture, DispatchDeferredEvent)
{
    float ref_delta_time{0.0F};

    auto lambda = [&ref_delta_time](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda);
    polos::communication::DispatchDefer<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    // Event should not be dispatched yet
    EXPECT_EQ(ref_delta_time, 0.0F);

    polos::communication::DispatchDeferredEvents();

    EXPECT_EQ(ref_delta_time, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, DispatchMultipleDeferredEvents)
{
    float ref_delta_time_1{0.0F};
    float ref_delta_time_2{0.0F};

    auto lambda_1 = [&ref_delta_time_1](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time_1 = tEvent.deltaTime;
    };

    auto lambda_2 = [&ref_delta_time_2](polos::communication::EngineUpdate& tEvent) {
        ref_delta_time_2 = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda_1);
    polos::communication::Subscribe<polos::communication::EngineUpdate>(lambda_2);
    polos::communication::DispatchDefer<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    // Event should not be dispatched yet
    EXPECT_EQ(ref_delta_time_1, 0.0F);
    EXPECT_EQ(ref_delta_time_2, 0.0F);

    polos::communication::DispatchDeferredEvents();

    EXPECT_EQ(ref_delta_time_1, kExpectedDeltaTime);
    EXPECT_EQ(ref_delta_time_2, kExpectedDeltaTime);
}

}// namespace
