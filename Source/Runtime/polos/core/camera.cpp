
#include "camera.h"

namespace polos
{
    Camera* Camera::s_Instance;

    Camera::Camera(glm::vec3 p_Position, glm::vec3 p_WorldUp, float p_Yaw, float p_Pitch)
        : position{p_Position},
          front{glm::vec3{0.0f, 0.0f, -1.0f}},
          up{},
          right{},
          worldUp{p_WorldUp},
          yaw{p_Yaw},
          pitch{p_Pitch},
          movementSpeed{globals::k_Speed},
          mouseSensitivity{globals::k_Sensitivity},
          zoom{globals::k_Zoom}
    {
        s_Instance = this;
        
        static_cast<void>(globals::k_Yaw);
        static_cast<void>(globals::k_Pitch);
        update_camera_vectors();
    }
    
    void Camera::ProcessKeyboard(CameraMovement p_Direction, float p_DeltaTime)
    {
        float velocity = movementSpeed * p_DeltaTime;
        if (p_Direction == k_Forward)
            position += front * velocity;
        else if (p_Direction == k_Backward)
            position -= front * velocity;
        if (p_Direction == k_Left)
            position -= right * velocity;
        else if (p_Direction == k_Right)
            position += right * velocity;
        if (p_Direction == k_Up)
            position += up * velocity;
        else if (p_Direction == k_Down)
            position -= up * velocity;
    }
    
    void Camera::ProcessMouseMovement(float p_XOffset, float p_YOffset, GLboolean p_ConstrainPitch)
    {
        p_XOffset *= mouseSensitivity;
        p_YOffset *= mouseSensitivity;
    
        yaw   += p_XOffset;
        pitch += p_YOffset;
    
        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (p_ConstrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
    
        // update Front, Right and Up Vectors using the updated Euler angles
        update_camera_vectors();
    }
    
    void Camera::ProcessMouseScroll(float p_YOffset)
    {
        zoom -= static_cast<float>(p_YOffset);
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
        // normalize the vectors, because their length gets closer to 0 the
        // more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(p_front, worldUp));
        up    = glm::normalize(glm::cross(right, p_front));
    }
}