
#include "camera_controller.h"

#include <imgui/imgui.h>

#include "polos/core/event_bus.h"
#include "polos/utils/key_code.h"
#include "polos/utils/mouse_code.h"
#include "polos/core/scene/scene_view.h"
#include "polos/core/ecs/sets/camera_set.h"

namespace polos
{
    CameraController* CameraController::s_Instance;

    CameraController::CameraController()
    {
        s_Instance = this;

        SUB_TO_EVENT_MEM_FUN(key_press,    on_key_press);
        SUB_TO_EVENT_MEM_FUN(key_release,  on_key_release);
        SUB_TO_EVENT_MEM_FUN(mouse_move,   on_mouse_move);
        SUB_TO_EVENT_MEM_FUN(mouse_scroll, on_mouse_scroll);
        SUB_TO_EVENT_MEM_FUN(mouse_button_press, on_mouse_button_press);
        SUB_TO_EVENT_MEM_FUN(mouse_button_release, on_mouse_button_release);

        SUB_TO_EVENT_MEM_FUN(engine_update, update);
    }

    CameraController::CameraController(Scene* p_Scene)
        : m_Scene{p_Scene}
    {
        s_Instance = this;

        SUB_TO_EVENT_MEM_FUN(key_press, on_key_press);
        SUB_TO_EVENT_MEM_FUN(key_release, on_key_release);
        SUB_TO_EVENT_MEM_FUN(mouse_move, on_mouse_move);
        SUB_TO_EVENT_MEM_FUN(mouse_scroll, on_mouse_scroll);
        SUB_TO_EVENT_MEM_FUN(mouse_button_press, on_mouse_button_press);
        SUB_TO_EVENT_MEM_FUN(mouse_button_release, on_mouse_button_release);

        SUB_TO_EVENT_MEM_FUN(engine_update, update);
    }

    void CameraController::AttachScene(Scene* p_Scene)
    {
        s_Instance->m_Scene = p_Scene;
    }

    void CameraController::update(engine_update& p_Event)
    {
        for (auto set : SceneView<ecs::camera_set>(*m_Scene))
        {
            CameraType cam_type = set.cameraComponent->type;
            if (set.cameraComponent->isEditor && (m_MouseButton & k_MouseButtonRight) == k_MouseButtonRight)
            {
                if (cam_type == CameraType::k_Perspective)
                {
                    // == Rotation ==
                    glm::vec3 new_rotation = (m_MouseMoveDelta * set.cameraComponent->mouseSensitivity) + set.transformComponent->rotation;

                    // make sure that when pitch is out of bounds, screen doesn't get flipped
                    if (new_rotation.x > k_PitchConstraint)
                        new_rotation.x = k_PitchConstraint;
                    if (new_rotation.x < -k_PitchConstraint)
                        new_rotation.x = -k_PitchConstraint;
                
                    set.Rotate(new_rotation);

                    // == Moving ==
                    float velocity = set.cameraComponent->movementSpeed * p_Event.deltaTime;
                    if ((m_Key & k_KeyA) == k_KeyA)
                        set.transformComponent->position -= set.cameraComponent->right * velocity;
                    if ((m_Key & k_KeyD) == k_KeyD)
                        set.transformComponent->position += set.cameraComponent->right * velocity;
                    if ((m_Key & k_KeyW) == k_KeyW)
                        set.transformComponent->position += set.cameraComponent->up * velocity;
                    if ((m_Key & k_KeyS) == k_KeyS)
                        set.transformComponent->position -= set.cameraComponent->up * velocity;
                }

                if (cam_type == CameraType::k_Ortho)
                {
                    glm::vec3 move_vector = m_MouseMoveDelta * set.cameraComponent->movementSpeed;

                    auto exp = std::exp2(-1.0f * p_Event.deltaTime);

                    set.transformComponent->position.x = std::lerp(set.transformComponent->position.x + move_vector.x, set.transformComponent->position.x, exp);
                    set.transformComponent->position.y = std::lerp(set.transformComponent->position.y + move_vector.y, set.transformComponent->position.y, exp);
                    set.transformComponent->position.z = std::lerp(set.transformComponent->position.z + move_vector.z, set.transformComponent->position.z, exp);
                }

                //if (p_Direction == k_Forward)
                //    set.transformComponent->position += set.cameraComponent->front * velocity;
                //else if (p_Direction == k_Backward)
                //    set.transformComponent->position -= set.cameraComponent->front * velocity;
            }
            m_MouseMoveDelta = glm::vec3(0.0f);
        }
    }

    void CameraController::on_key_press(key_press& p_Event)
    {
        int32 current_key = 0;

        switch (p_Event.key)
        {
        case KEY_W:
            current_key = k_KeyW;
            break;
        case KEY_S:
            current_key = k_KeyS;
            break;
        case KEY_A:
            current_key = k_KeyA;
            break;
        case KEY_D:
            current_key = k_KeyD;
            break;
        }

        m_Key |= current_key;
    }
    
    void CameraController::on_key_release(key_release& p_Event)
    {
        int32 current_key = 0;

        switch (p_Event.key)
        {
        case KEY_W:
            current_key = k_KeyW;
            break;
        case KEY_S:
            current_key = k_KeyS;
            break;
        case KEY_A:
            current_key = k_KeyA;
            break;
        case KEY_D:
            current_key = k_KeyD;
            break;
        }

        m_Key &= ~current_key;
    }
    
    void CameraController::on_mouse_move(mouse_move& p_Event)
    {
        if (m_MouseButton == 0)
            return;

        // TODO: Get the editor's framebuffer dimensions via a new event.
        if (m_MouseLetGo)
        {
            m_MouseMoveDelta = glm::vec3{0.0f};
            m_MouseLetGo = false;
        }
        else
            m_MouseMoveDelta = glm::vec3{m_CursorPosX - p_Event.x, p_Event.y - m_CursorPosY, 0.0f};

        m_CursorPosX = p_Event.x;
        m_CursorPosY = p_Event.y;
    }
    
    void CameraController::on_mouse_scroll(mouse_scroll&)
    {}

    void CameraController::on_mouse_button_press(mouse_button_press & p_Event)
    {
        int32 current_button = 0;

        switch (p_Event.button)
        {
        case MOUSE_BUTTON_LEFT:
            current_button = k_MouseButtonLeft;
            break;
        case MOUSE_BUTTON_RIGHT:
            current_button = k_MouseButtonRight;
            break;
        case MOUSE_BUTTON_MIDDLE:
            current_button = k_MouseButtonMiddle;
            break;
        }

        m_MouseButton |= current_button;
    }

    void CameraController::on_mouse_button_release(mouse_button_release & p_Event)
    {
        int32 current_button = 0;

        switch (p_Event.button)
        {
        case MOUSE_BUTTON_LEFT:
            current_button = k_MouseButtonLeft;
            break;
        case MOUSE_BUTTON_RIGHT:
            current_button = k_MouseButtonRight;
            m_MouseLetGo = true;
            break;
        case MOUSE_BUTTON_MIDDLE:
            current_button = k_MouseButtonMiddle;
            break;
        }

        m_MouseButton &= ~current_button;
    }
}