#pragma once

#include <glad/glad.h>

#include "polos/core/scene/scene.h"
#include "polos/core/events/input/input_events.h"
#include "polos/core/camera_type.h"
#include "polos/core/events/engine/engine_update.h"

namespace polos
{
    class CameraController
    {
    private:
        static constexpr int32 k_KeyW{1 << 0};
        static constexpr int32 k_KeyS{1 << 1};
        static constexpr int32 k_KeyA{1 << 2};
        static constexpr int32 k_KeyD{1 << 3};

        static constexpr int32 k_MouseButtonLeft{1 << 0};
        static constexpr int32 k_MouseButtonRight{1 << 1};
        static constexpr int32 k_MouseButtonMiddle{1 << 2};

        static constexpr float k_PitchConstraint{89.9f};
    public:
        CameraController();
        explicit CameraController(Scene* p_Scene);
    public:
        static void AttachScene(Scene* p_Scene);
    private:
        void update(engine_update& p_Event);

        void on_key_press(key_press& p_Event);
        void on_key_release(key_release& p_Event);

        void on_mouse_move(mouse_move& p_Event);
        void on_mouse_scroll(mouse_scroll& p_Event);

        void on_mouse_button_press(mouse_button_press& p_Event);
        void on_mouse_button_release(mouse_button_release& p_Event);
    private:
        static CameraController* s_Instance;

        Scene* m_Scene{};

        int32 m_Key{};
        int32 m_MouseButton{};
        bool  m_MouseLetGo{true};

        float m_CursorPosX{};
        float m_CursorPosY{};
        glm::vec3 m_MouseMoveDelta{};
    };
}
