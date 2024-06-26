
#ifdef GUI_USE_IMGUI

#include "polos/gui/gui.h"

#include <imgui/imgui_backend.h>

#include <GLFW/glfw3.h>

namespace polos
{
    void* Gui::s_Context;

    void Gui::Setup()
    {
        IMGUI_CHECKVERSION();
        s_Context = reinterpret_cast<void*>(ImGui::CreateContext());
        
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.FontGlobalScale = 1.8f;

        ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
        
        auto* window = glfwGetCurrentContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    
        ImGui::StyleColorsDark();

        auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header]        = ImVec4{  0.2f,  0.205f,  0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{  0.3f,  0.305f,  0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
		// Buttons
		colors[ImGuiCol_Button]        = ImVec4{  0.2f,  0.205f,  0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{  0.3f,  0.305f,  0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg]        = ImVec4{  0.2f,  0.205f,  0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{  0.3f,  0.305f,  0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab]                = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered]         = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive]          = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused]       = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{  0.2f,  0.205f,  0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg]          = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive]    = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
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

    void* Gui::GetContext()
    {
        return s_Context;
    }
}

#endif /* GUI_USE_IMGUI */