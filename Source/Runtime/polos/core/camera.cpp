#include "polos/polos_pch.h"

#include "camera.h"

namespace polos
{
    constexpr float globals::g_Yaw         = -90.0f;
    constexpr float globals::g_Pitch       =   0.0f;
    constexpr float globals::g_Speed       =   2.5f;
    constexpr float globals::g_Sensitivity =   0.1f;
    constexpr float globals::g_Zoom        =  45.0f;

    Camera* Camera::s_Instance;
    
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position{position},
          front{glm::vec3{0.0f, 0.0f, -1.0f}},
          up{},
          right{{}},
          worldUp(up),
          yaw(yaw),
          pitch(pitch),
          movementSpeed(globals::g_Speed),
          mouseSensitivity(globals::g_Sensitivity),
          zoom(globals::g_Zoom)
    {
        s_Instance = this;

        static_cast<void>(globals::g_Yaw);
        static_cast<void>(globals::g_Pitch);
        update_camera_vectors();
    }
    
    void Camera::ProcessKeyboard(CameraMovement direction, float delta_time)
    {
        float velocity = movementSpeed * delta_time;
        if (direction == kForward)
            position += front * velocity;
        else if (direction == kBackward)
            position -= front * velocity;
        if (direction == kLeft)
            position += right * velocity;
        else if (direction == kRight)
            position -= right * velocity;
        if (direction == kUp)
            position -= up * velocity;
        else if (direction == kDown)
            position += up * velocity;
    }
    
    void Camera::ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch)
    {
        x_offset *= mouseSensitivity;
        y_offset *= mouseSensitivity;
    
        yaw   += x_offset;
        pitch += y_offset;
    
        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
    
        // update Front, Right and Up Vectors using the updated Euler angles
        update_camera_vectors();
    }
    
    void Camera::ProcessMouseScroll(float y_offset)
    {
        zoom -= static_cast<float>(y_offset);
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }
    
    glm::mat4 Camera::GetViewMatrix()
    {
        auto* cam = s_Instance;
        return glm::lookAt(cam->position, cam->position + cam->front, cam->up);
    }
    
    void Camera::update_camera_vectors()
    {
        // calculate the new Front vector
        glm::vec3 p_front;
        p_front.x = glm::cos(yaw) * glm::cos(pitch);
        p_front.y = glm::sin(pitch);
        p_front.z = glm::sin(yaw) * glm::cos(pitch);
        p_front = glm::normalize(p_front);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(p_front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up    = glm::normalize(glm::cross(right, p_front));
    }
}