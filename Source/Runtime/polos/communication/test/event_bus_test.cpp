///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/end_frame.hpp"
#include "polos/communication/engine_update.hpp"
#include "polos/communication/event_bus.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace polos::communication
{
class EventBusTestFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ResetEventBus();
    }

    void TearDown() override
    {
        ResetEventBus();
    }

    static constexpr float kExpectedDeltaTime{44.0F};
private:
    static auto ResetEventBus() -> void
    {
        auto& eventBus = EventBus::Instance();
        eventBus.mCallbacks.clear();
        eventBus.mDeferredEvents.clear();
    }
};

TEST_F(EventBusTestFixture, SubscribeToEventTest)
{
    float receivedDeltaTime{0.0F};

    auto subscriber = [&receivedDeltaTime](polos::communication::EngineUpdate& tEvent) {
        receivedDeltaTime = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(subscriber);
    polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(receivedDeltaTime, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, SubscribeMultipleTimes)
{
    float firstDeltaTime{0.0F};
    float secondDeltaTime{0.0F};

    auto firstSubscriber = [&firstDeltaTime](polos::communication::EngineUpdate& tEvent) {
        firstDeltaTime = tEvent.deltaTime;
    };

    auto secondSubscriber = [&secondDeltaTime](polos::communication::EngineUpdate& tEvent) {
        secondDeltaTime = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(firstSubscriber);
    polos::communication::Subscribe<polos::communication::EngineUpdate>(secondSubscriber);
    polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(firstDeltaTime, kExpectedDeltaTime);
    EXPECT_EQ(secondDeltaTime, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, DispatchWithoutSubscribers)
{
    std::size_t const totalDispatched =
        polos::communication::DispatchNow<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(totalDispatched, 0U);
}

TEST_F(EventBusTestFixture, DispatchDeferredEvent)
{
    float receivedDeltaTime{0.0F};

    auto subscriber = [&receivedDeltaTime](polos::communication::EngineUpdate& tEvent) {
        receivedDeltaTime = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(subscriber);
    polos::communication::DispatchDefer<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(receivedDeltaTime, 0.0F);

    polos::communication::DispatchDeferredEvents();

    EXPECT_EQ(receivedDeltaTime, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, DispatchMultipleDeferredEvents)
{
    float firstDeltaTime{0.0F};
    float secondDeltaTime{0.0F};

    auto firstSubscriber = [&firstDeltaTime](polos::communication::EngineUpdate& tEvent) {
        firstDeltaTime = tEvent.deltaTime;
    };

    auto secondSubscriber = [&secondDeltaTime](polos::communication::EngineUpdate& tEvent) {
        secondDeltaTime = tEvent.deltaTime;
    };

    polos::communication::Subscribe<polos::communication::EngineUpdate>(firstSubscriber);
    polos::communication::Subscribe<polos::communication::EngineUpdate>(secondSubscriber);
    polos::communication::DispatchDefer<polos::communication::EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(firstDeltaTime, 0.0F);
    EXPECT_EQ(secondDeltaTime, 0.0F);

    polos::communication::DispatchDeferredEvents();

    EXPECT_EQ(firstDeltaTime, kExpectedDeltaTime);
    EXPECT_EQ(secondDeltaTime, kExpectedDeltaTime);
}

TEST_F(EventBusTestFixture, ImmediateDispatchReportsSubscribersAndSharesEventChanges)
{
    std::vector<float> observedDeltaTimes;
    auto const         firstSubscriber  = Subscribe<EngineUpdate>([&](EngineUpdate& tEvent) {
        observedDeltaTimes.push_back(tEvent.deltaTime);
        tEvent.deltaTime += 2.0F;
    });
    auto const         secondSubscriber = Subscribe<EngineUpdate>([&](EngineUpdate& tEvent) {
        observedDeltaTimes.push_back(tEvent.deltaTime);
    });

    auto const dispatched = DispatchNow<EngineUpdate>(4.0F);

    std::vector<float> const expectedDeltaTimes{4.0F, 6.0F};
    EXPECT_NE(firstSubscriber, secondSubscriber);
    EXPECT_EQ(dispatched, 2U);
    EXPECT_EQ(observedDeltaTimes, expectedDeltaTimes);
}

TEST_F(EventBusTestFixture, DispatchOnlyNotifiesSubscribersOfTheMatchingEventType)
{
    unsigned engineUpdates{0U};
    unsigned frameEnds{0U};
    Subscribe<EngineUpdate>([&](EngineUpdate&) {
        ++engineUpdates;
    });
    Subscribe<EndFrame>([&](EndFrame&) {
        ++frameEnds;
    });

    auto const dispatchedUpdates = DispatchNow<EngineUpdate>(kExpectedDeltaTime);

    EXPECT_EQ(dispatchedUpdates, 1U);
    EXPECT_EQ(engineUpdates, 1U);
    EXPECT_EQ(frameEnds, 0U);

    auto const dispatchedFrameEnds = DispatchNow<EndFrame>();

    EXPECT_EQ(dispatchedFrameEnds, 1U);
    EXPECT_EQ(engineUpdates, 1U);
    EXPECT_EQ(frameEnds, 1U);
}

TEST_F(EventBusTestFixture, DeferredEventsRetainInsertionOrderAndAreNotReplayed)
{
    std::vector<float> receivedDeltaTimes;
    Subscribe<EngineUpdate>([&](EngineUpdate& tEvent) {
        receivedDeltaTimes.push_back(tEvent.deltaTime);
    });

    for (auto const deltaTime : {1.0F, 2.0F, 3.0F})
    {
        SCOPED_TRACE(deltaTime);
        EXPECT_EQ(DispatchDefer<EngineUpdate>(deltaTime), 1U);
    }

    EXPECT_TRUE(receivedDeltaTimes.empty());

    DispatchDeferredEvents();

    std::vector<float> const expectedDeltaTimes{1.0F, 2.0F, 3.0F};
    EXPECT_EQ(receivedDeltaTimes, expectedDeltaTimes);

    DispatchDeferredEvents();

    EXPECT_EQ(receivedDeltaTimes, expectedDeltaTimes);
}

TEST_F(EventBusTestFixture, ImmediateDispatchDoesNotFlushDeferredEvents)
{
    std::vector<float> receivedDeltaTimes;
    Subscribe<EngineUpdate>([&](EngineUpdate& tEvent) {
        receivedDeltaTimes.push_back(tEvent.deltaTime);
    });
    DispatchDefer<EngineUpdate>(1.0F);

    DispatchNow<EngineUpdate>(2.0F);

    std::vector<float> const expectedBeforeDrain{2.0F};
    EXPECT_EQ(receivedDeltaTimes, expectedBeforeDrain);

    DispatchDeferredEvents();

    std::vector<float> const expectedAfterDrain{2.0F, 1.0F};
    EXPECT_EQ(receivedDeltaTimes, expectedAfterDrain);
}

TEST_F(EventBusTestFixture, DeferredDispatchWithoutSubscribersReportsZero)
{
    auto const dispatched = DispatchDefer<EngineUpdate>(kExpectedDeltaTime);
    DispatchDeferredEvents();

    EXPECT_EQ(dispatched, 0U);
    EXPECT_EQ(DispatchNow<EngineUpdate>(kExpectedDeltaTime), 0U);
}

}// namespace polos::communication
