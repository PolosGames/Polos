///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/communication/end_frame.hpp"
#include "polos/communication/event_bus.hpp"
#include "polos/communication/key_press.hpp"
#include "polos/communication/key_release.hpp"
#include "polos/communication/mouse_input.hpp"
#include "polos/communication/mouse_move.hpp"
#include "polos/communication/process_input.hpp"
#include "polos/platform/input_state.hpp"

#include <gtest/gtest.h>

#include <GLFW/glfw3.h>

namespace
{

namespace input = polos::platform::input;
using namespace polos::communication;

class InputStateTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    { input::Initialize(); }

    void SetUp() override
    { input::g_input_state = {}; }
};

TEST_F(InputStateTest, KeyEventsRetainEdgesUntilInputIsProcessed)
{
    DispatchNow<KeyPress>(GLFW_KEY_W);
    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_TRUE(input::IsKeyPressed(GLFW_KEY_W));

    DispatchNow<ProcessInput>();
    EXPECT_TRUE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_FALSE(input::IsKeyPressed(GLFW_KEY_W));

    DispatchNow<KeyRelease>(GLFW_KEY_W);
    EXPECT_FALSE(input::IsKeyDown(GLFW_KEY_W));
    EXPECT_TRUE(input::IsKeyReleased(GLFW_KEY_W));

    DispatchNow<ProcessInput>();
    EXPECT_FALSE(input::IsKeyReleased(GLFW_KEY_W));
}

TEST_F(InputStateTest, MouseEventsUpdateButtonsAndResetMovementAtEndFrame)
{
    DispatchNow<MouseMove>(10.0, 20.0);
    DispatchNow<MouseMove>(13.0, 18.0);
    EXPECT_FLOAT_EQ(input::g_input_state.mousePos.x, 13.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mousePos.y, 18.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mouseDelta.x, 3.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mouseDelta.y, -2.0F);

    DispatchNow<MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
    EXPECT_TRUE(input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT));
    DispatchNow<MouseInput>(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);
    EXPECT_FALSE(input::IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT));

    DispatchNow<EndFrame>();
    EXPECT_FLOAT_EQ(input::g_input_state.mouseDelta.x, 0.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mouseDelta.y, 0.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mousePos.x, 13.0F);
    EXPECT_FLOAT_EQ(input::g_input_state.mousePos.y, 18.0F);
}

}// namespace
