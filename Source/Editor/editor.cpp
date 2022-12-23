#include "polos/polos_pch.h"

#include <imgui.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "polos/core/update_queue.h"
#include "polos/context/shader_lib.h"
#include "polos/context/vertex.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"

#include "editor.h"

#include <polos.h>

namespace polos
{

    // 2D Texture Rectangle

    static constexpr std::array<vertex, 8> vertices {
            vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f), glm::vec3{1.0f}}, // Bottom-left
            vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f), glm::vec3{1.0f}}, // Bottom-right
            vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f), glm::vec3{1.0f}}, // Top-right
            vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f), glm::vec3{1.0f}}, // Top-left
    };

    static constexpr std::array<uint32, 6> indices {
            0, 1, 2,
            2, 3, 0,
    };

    // !2D Texture Rectangle

    Editor::Editor()
        : cube{vertices, indices}
    {
        UPDATE_Q_MEM_ADD_LAST(Update);
        
        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        
        basic_color = ShaderLib::Get("basic_color"_sid);

        model  = glm::mat4(1.0f);

        app_window     = WindowSystem::GetAppWindowGUID();
        popup_window   = INT_MIN;

        auto win_props = WindowSystem::GetAppWindowProps();

        pos        = glm::vec3(0.0f, 0.0f, 0.0f);
        slider_pos = pos;

        open = true;
    }

    Editor::~Editor()
    {
    }

    void Editor::Update(float delta_time)
    {
        static_cast<void>(delta_time);

        model = glm::translate(model, (pos - slider_pos));
        pos = slider_pos;

        basic_color.Use();
        basic_color.SetUniform("u_View"_sid, view);
        basic_color.SetUniform("u_Projection"_sid, projection);

        WindowSystem::SwitchWindow(app_window);
        basic_color.SetUniform("u_Model"_sid, model);
        cube.Draw();

        ImGui::Begin("First box");
        ImGui::SliderFloat3("Position 1", glm::value_ptr(slider_pos), -5.0f, 5.0f);
        ImGui::End();

        //auto const* viewport = ImGui::GetMainViewport();

        //ImGui::DockSpaceOverViewport(viewport);

        //ImGui::ShowDemoWindow(&open);
    }

    Application* CreateApplication(void* ptr)
    {
        window_props props;
        props.title = "Hello";
        props.width = 1280;
        props.height = 720;
        props.refreshRate = 60;
        props.vsync = true;
        props.fullscreen = false;

        auto a = WindowSystem::NewWindow(props);

        Application* app = !ptr ? new Editor() : new (ptr) Editor();
        return app;
    }
}