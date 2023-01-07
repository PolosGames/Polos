#include "polos/polos_pch.h"

#include <bitset>

#include <imgui.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <GLFW/glfw3.h>

#include "polos/core/update_queue.h"
#include "polos/context/shader_lib.h"
#include "polos/context/vertex.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"
#include "polos/renderer/renderer.h"
#include "polos/renderer/shapes/shapes2d_transform.h"

#include "editor.h"

#include <polos.h>

namespace polos
{
    Editor::Editor()
        : camera{{0.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}}
    {
        UPDATE_Q_MEM_ADD_LAST(Update);
        
        app_window     = WindowSystem::GetAppWindowGUID();

        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        ShaderLib::Load("resources/shaders/texture.vert", "resources/shaders/texture.frag");

        m_TextureShader = &ShaderLib::Get("texture"_sid);
        model       = glm::mat4(1.0f);
        key = 0;

        texture = Texture::Load("resources/textures/linux-22621.png");

        shapes::MoveShape2DToPosition(model, glm::vec3{0.0f, 0.0f, 1.0f});

        SUB_TO_EVENT_MEM_FUN(mouse_move, OnMouseMove);
        SUB_TO_EVENT_MEM_FUN(key_press, OnKeyPress);
        SUB_TO_EVENT_MEM_FUN(key_release, OnKeyRelease);
    }

    Editor::~Editor()
    {
    }

    void Editor::Update(float delta_time)
    {
        static_cast<void>(delta_time);

        CameraMovement l_camera_move = kNone;

        if ((key & (1)) != 0)
        {
            l_camera_move = kUp;
        }
        else if ((key & (2)) != 0)
        {
            l_camera_move = kDown;
        }
        camera.ProcessKeyboard(l_camera_move, delta_time);
        if ((key & (4)) != 0)
        {
            l_camera_move = kLeft;
        }
        else if ((key & (8)) != 0)
        {
            l_camera_move = kRight;
        }
        camera.ProcessKeyboard(l_camera_move, delta_time);

        RenderTexture2D(model, texture, *m_TextureShader);

        //ImGui::Begin("First box");
        //ImGui::SliderFloat3("Position 1", glm::value_ptr(slider_pos), -5.0f, 5.0f);
        //ImGui::End();

        //auto const* viewport = ImGui::GetMainViewport();

        //ImGui::DockSpaceOverViewport(viewport);

        //ImGui::ShowDemoWindow(&open);
    }

    void Editor::OnKeyPress(key_press& e)
    {
        int32 current_key = 0;

        switch (e.key)
        {
        case GLFW_KEY_W:
            current_key = 1;
            break;
        case GLFW_KEY_S:
            current_key = 2;
            break;
        case GLFW_KEY_A:
            current_key = 4;
            break;
        case GLFW_KEY_D:
            current_key = 8;
            break;
        }

        key |= current_key;
    }

    void Editor::OnKeyRelease(key_release& e)
    {
        int32 current_key = 0;
        
        switch (e.key)
        {
        case GLFW_KEY_W:
            current_key = 1;
            break;
        case GLFW_KEY_S:
            current_key = 2;
            break;
        case GLFW_KEY_A:
            current_key = 4;
            break;
        case GLFW_KEY_D:
            current_key = 8;
            break;
        }
        
        key &= ~current_key;
    }

    void Editor::OnMouseMove(mouse_move&)
    {
    }

    Application* CreateApplication(void* ptr)
    {
        window_props props;
        props.title = "Hello";
        props.width = 1920;
        props.height = 1080;
        props.refreshRate = 120;
        props.vsync = false;
        props.fullscreen = false;

        auto a = WindowSystem::NewWindow(props);

        Application* app = !ptr ? new Editor() : new (ptr) Editor();
        return app;
    }
}