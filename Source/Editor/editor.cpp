#include "polos/polos_pch.h"

#include <imgui.h>

#include "polos/core/update_queue.h"
#include "polos/context/shader_lib.h"
#include "polos/context/vertex.h"
#include "polos/core/event_bus.h"

#include "editor.h"

#include <polos.h>

namespace polos
{
    static constexpr std::array<vertex, 8> vertices {
            // Front
            vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{1.0f, 0.0f, 0.0f}}, // Bottom-left
            vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 1.0f, 0.0f}}, // Bottom-right
            vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 0.0f, 1.0f}}, // Top-right
            vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.5f, 0.5f, 0.5f}}, // Top-left
        
            // Back
            vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 1.0f, 0.0f}}, // Bottom-left
            vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{1.0f, 0.0f, 0.0f}}, // Bottom-right
            vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.5f, 0.5f, 0.5f}}, // Top-right
            vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3{0.0f, 0.0f, 1.0f}}, // Top-left
    };

    static constexpr std::array<uint32, 36> indices {
            0, 1, 2,   2, 3, 0, // Front
            4, 5, 6,   6, 7, 4, // Back
            5, 0, 3,   3, 6, 5, // left
            1, 4, 7,   7, 2, 1, // right
            3, 2, 7,   7, 6, 3, // top
            1, 0, 5,   5, 4, 1  // Bottom
    };

    Editor::Editor()
        : cube{vertices, indices}
    {
        UPDATE_Q_MEM_ADD_LAST(Editor, Update);
        
        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        
        basic_color = ShaderLib::Get("basic_color"_sid);

        model  = glm::mat4(1.0f);
        model2 = glm::mat4(1.0f);
        view   = glm::lookAt(
            glm::vec3(0.5f, 0.5f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        IWindow& window = Application::get_main_window();
        float aspect = static_cast<float>(window.Width()) / static_cast<float>(window.Height());
        projection = glm::perspective(
            45.0f,
            aspect,
            0.1f,
            1000.0f
        );
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
        pos = slider_pos;
        
        model2 = glm::translate(model2, (pos2 - slider_pos2));
        pos2 = slider_pos2;

        basic_color.Use();
        basic_color.SetUniform("u_View"_sid, view);
        basic_color.SetUniform("u_Projection"_sid, projection);

        basic_color.SetUniform("u_Model"_sid, model);
        cube.Draw();

        basic_color.SetUniform("u_Model"_sid, model2);
        cube.Draw();
        
        ImGui::Begin("First box");
        ImGui::SliderFloat3("Position", glm::value_ptr(slider_pos), -5.0f, 5.0f);
        ImGui::End();

        ImGui::Begin("Second box");
        ImGui::SliderFloat3("Position", glm::value_ptr(slider_pos2), -5.0f, 5.0f);
        ImGui::End();

        //auto const* viewport = ImGui::GetMainViewport();

        //ImGui::DockSpaceOverViewport(viewport);

        //ImGui::ShowDemoWindow(&open);
    }

    Application* CreateApplication(void* ptr)
    {
        Application* app = !ptr ? new Editor() : new (ptr) Editor();
        return app;
    }
}