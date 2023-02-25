#pragma once

#include <glad/glad.h>

namespace polos
{
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        k_Forward,
        k_Backward,
        k_Left,
        k_Right,
        k_Up,
        k_Down,
        k_None
    };
    
    namespace globals
    {
        inline constexpr float k_Yaw{-90.0f};
        inline constexpr float k_Pitch{0.0f};
        inline constexpr float k_Speed{2.5f};
        inline constexpr float k_Sensitivity{0.1f};
        inline constexpr float k_Zoom{45.0f};
    }

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera
    {
    public:
        // constructor with vectors
        Camera(glm::vec3 p_Position, glm::vec3 p_WorldUp, float p_Yaw = globals::k_Yaw, float p_Pitch = globals::k_Pitch);

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        static PL_NODISCARD glm::mat4 GetViewMatrix();
        
        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(CameraMovement p_Direction, float p_DeltaTime);
        
        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float p_XOffset, float p_YOffset, GLboolean p_ConstrainPitch = true);
        
        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float p_YOffset);
        
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
        static Camera* s_Instance;

        // calculates the front vector from the Camera's (updated) Euler Angles
        void update_camera_vectors();
    };
}
