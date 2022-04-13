#include "polos_pch.h"

#ifdef GUI_USE_IMGUI

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gui/gui.h"

namespace polos
{
    void Gui::Setup()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.FontGlobalScale = 1.5f;
        
        auto* window = glfwGetCurrentContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    
        ImGui::StyleColorsDark();
    }
    
    void Gui::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    void Gui::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            auto* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    
    void Gui::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

#endif /* GUI_USE_IMGUI */