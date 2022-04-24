#include "polos_pch.h"

#include <glad/glad.h>

#include "event_bus.h"
#include "events/events.h"
#include "update_queue.h"
#include "time/timer.h"
#include "gui/gui.h"

#include "application.h"

namespace polos
{
    Application::Application()
        : m_WindowInstance{IWindow::NewWindow()}, m_IsRunning{true}
    {
        EventBus::SubscribeToEvent<window_close, Application, &Application::on_window_close>(this);
    }

    void Application::Run()
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
//        int width{0};
//        int height{0};
//        int channels{0};
//        byte* data = stbi_load("resources/textures/linux-22621.png", &width, &height, &channels, 4);
//
//        GLuint image_texture;
//        glCreateTextures(GL_TEXTURE_2D, 1, &image_texture);
//
//        // Setup filtering parameters for display
//        glTextureParameteri(image_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTextureParameteri(image_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTextureParameteri(image_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTextureParameteri(image_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//        // Upload pixelsw into texture
//        glTextureStorage2D(image_texture, 1, GL_RGBA8, width, height);
//        glTextureSubImage2D(image_texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
//        stbi_image_free(data);
        
        float delta_time{};
        int64 end  { time::Timer::Now() };
        int64 start{ time::Timer::Now() };
        
        glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
        
        Gui::Setup();
        while (m_IsRunning)
        {
            end = time::Timer::Now();
            delta_time = (end - start) * 0.001f * 0.001f * 0.001f;
            start = time::Timer::Now();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            Gui::Begin();
            
            UpdateQueue::Update(delta_time);
    
            Gui::End();
            
            m_WindowInstance->Update();
        }
        
        Gui::Shutdown();
    }
    
    void Application::on_window_close(window_close &e)
    {
        m_IsRunning = false;
        m_WindowInstance->Shutdown();
    }
    
    Application::~Application()
    {
    }
    
    IWindow& Application::get_main_window()
    {
        return *m_WindowInstance;
    }
} // namespace polos