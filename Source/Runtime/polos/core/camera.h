#pragma once

#include <glad/glad.h>

namespace polos
{
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        kForward,
        kBackward,
        kLeft,
        kRight
    };
    
    namespace globals
    {
        extern const float g_Yaw;
        extern const float g_Pitch;
        extern const float g_Speed;
        extern const float g_Sensitivity;
        extern const float g_Zoom;
    }

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera
    {
    public:
        // constructor with vectors
        Camera(glm::vec3 position, glm::vec3 up, float yaw = globals::g_Yaw, float pitch = globals::g_Pitch);

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        PL_NODISCARD glm::mat4 GetViewMatrix() const;
        
        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CameraMovement direction, float delta_time);
        
        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch = true);
        
        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float y_offset);
        
        // camera Attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;
        // euler Angles
        float yaw;
        float pitch;
        // camera options
        float movementSpeed;
        float mouseSensitivity;
        float zoom;
    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void update_camera_vectors();
    };
}
