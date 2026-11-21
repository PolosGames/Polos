///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/event_bus.hpp"
#include "polos/core/events/end_frame.hpp"
#include "polos/input/events/key_press.hpp"
#include "polos/input/events/key_release.hpp"
#include "polos/input/events/mouse_input.hpp"
#include "polos/input/events/mouse_move.hpp"
#include "polos/input/events/process_input.hpp"
#include "polos/input/input_state.hpp"

#include <gtest/gtest.h>

#include <GLFW/glfw3.h>

namespace
{
namespace input = polos::input;
using namespace polos::communication;

class InputStateTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        input::Initialize();
    }

    void SetUp() override
    {
        input::gInputState = {};
    }
};

TEST_F(InputStateTest, KeyEventsRetainEdgesUntilInputIsProcessed)
{
    DispatchNow<input::KeyPress>(GLFW_KEY_W);

    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_TRUE(input::IsKeyPressed(GLFW_KEY_W));

    DispatchNow<input::ProcessInput>();

    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_FALSE(input::IsKeyPressed(GLFW_KEY_W));

    DispatchNow<input::KeyRelease>(GLFW_KEY_W);

    EXPECT_FALSE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_TRUE(input::IsKeyReleased(GLFW_KEY_W));

    DispatchNow<input::ProcessInput>();

    EXPECT_FALSE(input::IsKeyReleased(GLFW_KEY_W));
}

TEST_F(InputStateTest, MouseEventsUpdateButtonsAndResetMovementAtEndFrame)
{
    DispatchNow<input::MouseMove>(10.0, 20.0);
    DispatchNow<input::MouseMove>(13.0, 18.0);

    EXPECT_FLOAT_EQ(input::gInputState.mousePos.x, 13.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mousePos.y, 18.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mouseDelta.x, 3.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mouseDelta.y, -2.0F);

    DispatchNow<input::MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);

    EXPECT_TRUE(input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT));

    DispatchNow<input::MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);

    EXPECT_FALSE(input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT));

    DispatchNow<polos::core::EndFrame>();

    EXPECT_FLOAT_EQ(input::gInputState.mouseDelta.x, 0.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mouseDelta.y, 0.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mousePos.x, 13.0F);
    EXPECT_FLOAT_EQ(input::gInputState.mousePos.y, 18.0F);
}

TEST_F(InputStateTest, HeldKeyDoesNotGenerateAnotherPressUntilReleased)
{
    DispatchNow<input::KeyPress>(GLFW_KEY_A);
    DispatchNow<input::ProcessInput>();

    DispatchNow<input::KeyPress>(GLFW_KEY_A);

    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_A));
    EXPECT_FALSE(input::IsKeyPressed(GLFW_KEY_A));
    EXPECT_FALSE(input::IsKeyReleased(GLFW_KEY_A));

    DispatchNow<input::KeyRelease>(GLFW_KEY_A);
    DispatchNow<input::ProcessInput>();
    DispatchNow<input::KeyPress>(GLFW_KEY_A);

    EXPECT_TRUE(input::IsKeyPressed(GLFW_KEY_A));
    EXPECT_FALSE(input::IsKeyReleased(GLFW_KEY_A));
}

TEST_F(InputStateTest, ReleasingOneKeyDoesNotChangeOtherKeysOrMouseButtons)
{
    DispatchNow<input::KeyPress>(GLFW_KEY_A);
    DispatchNow<input::KeyPress>(GLFW_KEY_D);
    DispatchNow<input::MouseInput>(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    DispatchNow<input::ProcessInput>();

    DispatchNow<input::KeyRelease>(GLFW_KEY_A);

    EXPECT_TRUE(input::IsKeyReleased(GLFW_KEY_A));
    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_D));
    EXPECT_FALSE(input::IsKeyPressed(GLFW_KEY_D));
    EXPECT_TRUE(input::IsKeyDown(GLFW_MOUSE_BUTTON_LEFT));

    DispatchNow<polos::core::EndFrame>();

    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_D));
    EXPECT_TRUE(input::IsKeyDown(GLFW_MOUSE_BUTTON_LEFT));
}

}// namespace
