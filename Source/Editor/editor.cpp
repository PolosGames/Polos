#include "polos/polos_pch.h"

#include "editor.h"

#include <glad/glad.h>
#include <imgui.h>
#include <polos.h>

#include "polos/context/shader_lib.h"
#include "polos/context/vertex.h"
#include "polos/core/event_bus.h"
#include "polos/core/update_queue.h"
#include "polos/core/window_system.h"
#include "polos/memory/pool_allocator.h"

namespace polos
{
    static constexpr std::array<vertex, 8> vertices{
        // Front
        vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{1.0f, 0.0f, 0.0f}},// Bottom-left
        vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 1.0f, 0.0f}}, // Bottom-right
        vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 0.0f, 1.0f}},  // Top-right
        vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.5f, 0.5f, 0.5f}}, // Top-left

        // Back
        vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 1.0f, 0.0f}}, // Bottom-left
        vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{1.0f, 0.0f, 0.0f}},// Bottom-right
        vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.5f, 0.5f, 0.5f}}, // Top-right
        vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 0.0f, 1.0f}},  // Top-left
    };

    static constexpr std::array<uint32, 36> indices{
        0, 1, 2, 2, 3, 0,// Front
        4, 5, 6, 6, 7, 4,// Back
        5, 0, 3, 3, 6, 5,// left
        1, 4, 7, 7, 2, 1,// right
        3, 2, 7, 7, 6, 3,// top
        1, 0, 5, 5, 4, 1 // Bottom
    };

    Editor::Editor()
        : cube{vertices, indices}
    {
        UPDATE_Q_MEM_ADD_LAST(Update);

        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");

        basic_color = ShaderLib::Get("basic_color"_sid);

        model  = glm::mat4(1.0f);
        model2 = glm::mat4(1.0f);
        view   = glm::lookAt(
            glm::vec3(0.5f, 0.5f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        app_window   = WindowSystem::GetAppWindowGUID();
        popup_window = INT_MIN;

        auto win_props = WindowSystem::GetAppWindowProps();
        float aspect   = static_cast<float>(win_props->width) / static_cast<float>(win_props->height);
        projection     = glm::perspective(
            45.0f,
            aspect,
            0.1f,
            1000.0f);
        pos        = glm::vec3(0.0f, 0.0f, -3.0f);
        slider_pos = pos;

        pos2        = glm::vec3(2.0f, 0.0f, -3.0f);
        slider_pos2 = pos2;

        open = true;
    }

    Editor::~Editor()
    {
    }

    void Editor::Update(float delta_time)
    {
        static_cast<void>(delta_time);
        model = glm::translate(model, (pos - slider_pos));
        pos   = slider_pos;

        model2 = glm::translate(model2, (pos2 - slider_pos2));
        pos2   = slider_pos2;

        basic_color.Use();
        basic_color.SetUniform("u_View"_sid, view);
        basic_color.SetUniform("u_Projection"_sid, projection);

        WindowSystem::SwitchWindow(app_window);
        basic_color.SetUniform("u_Model"_sid, model);
        cube.Draw();

        basic_color.SetUniform("u_Model"_sid, model2);
        cube.Draw();

        ImGui::Begin("First box");
        ImGui::SliderFloat3("Position 1", glm::value_ptr(slider_pos), -5.0f, 5.0f);
        ImGui::SliderFloat3("Position 2", glm::value_ptr(slider_pos2), -5.0f, 5.0f);
        if (ImGui::Button("New Window", ImVec2{100, 40}))
        {
            window_props props;
            props.width       = 720;
            props.height      = 360;
            props.title       = "Another";
            props.vsync       = true;
            props.refreshRate = 60;
            props.fullscreen  = false;

            popup_window = WindowSystem::NewWindow(props);
            WindowSystem::SwitchWindow(popup_window);
            glClearColor(0.3f, 1.0f, .502f, 1.0f);
        }
        ImGui::End();

        //auto const* viewport = ImGui::GetMainViewport();

        //ImGui::DockSpaceOverViewport(viewport);

        //ImGui::ShowDemoWindow(&open);
    }

    Application* CreateApplication(void* p_PlacementPtr)
    {
        window_props props;
        props.title       = "Hello";
        props.width       = 1280;
        props.height      = 720;
        props.refreshRate = 60;
        props.vsync       = true;
        props.fullscreen  = false;

        auto a = WindowSystem::NewWindow(props);

        Application* app = p_PlacementPtr != nullptr ? new (p_PlacementPtr) Editor() : new Editor();
        return app;
    }
}// namespace polos