#include "editor.h"

#include <glad/glad.h>
#include <imgui/imgui_backend.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "polos/core/update_queue.h"
#include "polos/graphics/shader_lib.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"
#include "polos/graphics/renderer.h"
#include "polos/graphics/shapes/shapes2d_transform.h"
#include "polos/core/ecs/components/components.h"
#include "polos/core/scene/scene_view.h"

#include <polos.h>

namespace polos
{
    Editor::Editor()
        : m_EditorCamera{{0.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}}
        , m_GuiFontScale{1.8f}
        , m_Key{0}
        , m_Model{glm::mat4(1.0f)}
        , m_ScaledModel{glm::mat4(1.0f)}
        , m_EditorFramebufferAspectRatioBefore{0.0f}
        , m_EditorFramebufferDimensions{ImVec2{0.0f, 0.0f}}
        , m_EditorFramebufferUVCoords1{ImVec2{0.0f, 1.0f}}
        , m_EditorFramebufferUVCoords2{ImVec2{1.0f, 0.0f}}
    {
        UPDATE_Q_MEM_ADD_LAST(Update);
        
        m_AppWindow     = WindowSystem::GetAppWindowGUID();
        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        ShaderLib::Load("resources/shaders/texture.vert", "resources/shaders/texture.frag");
        m_ShaderTexture    = &ShaderLib::Get("texture"_sid);
        m_ShaderBasicColor = &ShaderLib::Get("basic_color"_sid);
        m_Texture = Texture::Load("resources/textures/linux-22621.png");

        Optional<window_props> app_window_props = WindowSystem::GetWindowProps(m_AppWindow);
        m_AppWindowWidth  = app_window_props->width;
        m_AppWindowHeight = app_window_props->height;
        m_AspectRatio     = static_cast<float>(m_AppWindowWidth) / m_AppWindowHeight;

        m_EditorFramebuffer.Initialize(m_AppWindowWidth, m_AppWindowHeight);
        m_EditorFramebuffer.Bind();
        glViewport(0, 0, m_AppWindowWidth, m_AppWindowHeight);
        m_EditorFramebuffer.Unbind();

        m_TextureEntity = m_Scene.NewEntity();

        m_TextureEntityTransformComponent = m_Scene.Assign<ecs::transform_component>(m_TextureEntity);
        m_TextureEntityTransformComponent->position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        m_TextureEntityTransformComponent->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_TextureEntityTransformComponent->scale    = glm::vec3(1.0f, 1.0f, 1.0f);

        m_TextureEntityTexture2DComponent = m_Scene.Assign<ecs::texture2d_component>(m_TextureEntity);
        m_TextureEntityTexture2DComponent->texture = m_Texture;
        m_TextureEntityTexture2DComponent->uvCoordinates = glm::vec2(1, 1);

        auto* info       = m_Scene.Get<ecs::info_component>(m_TextureEntity);
        std::string name = "Texture";
        std::ranges::copy(name, info->name);

        // move to initial position
        shapes::MoveShape2DToPosition(m_Model, m_TextureEntityTransformComponent->position);
        m_ScaledModel = shapes::ScaleShape2D(m_Model, m_TextureEntityTransformComponent->scale.x, m_TextureEntityTransformComponent->scale.y);

        SUB_TO_EVENT_MEM_FUN(mouse_move, OnMouseMove);
        SUB_TO_EVENT_MEM_FUN(key_press, OnKeyPress);
        SUB_TO_EVENT_MEM_FUN(key_release, OnKeyRelease);

        m_SelectedEntity = INVALID_ENTITY;
    }

    Editor::~Editor()
    {
    }

    void Editor::Update(float p_DeltaTime)
    {
        CameraMovement camera_move{k_None};

        if ((m_Key & (1)) != 0)
        {
            camera_move = k_Up;
        }
        else if ((m_Key & (2)) != 0)
        {
            camera_move = k_Down;
        }
        m_EditorCamera.ProcessKeyboard(camera_move, p_DeltaTime);
        if ((m_Key & (4)) != 0)
        {
            camera_move = k_Left;
        }
        else if ((m_Key & (8)) != 0)
        {
            camera_move = k_Right;
        }
        m_EditorCamera.ProcessKeyboard(camera_move, p_DeltaTime);

        m_EditorFramebuffer.Clear();

        m_EditorFramebuffer.Bind();
        RenderTexture2D(m_ScaledModel, m_Texture, *m_ShaderTexture);
        //RenderRectangle(m_Model, *m_ShaderBasicColor);
        m_EditorFramebuffer.Unbind();

        ImGui::ShowDemoWindow();

        RenderGUI(p_DeltaTime);
    }

    void Editor::RenderGUI(float p_DeltaTime)
    {
        PL_VOID_CAST(p_DeltaTime);

        auto const* viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(viewport);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene"))
            {
                ImGui::MenuItem("Save");
                ImGui::MenuItem("Open");
                ImGui::MenuItem("Import...");
                ImGui::MenuItem("Export...");
                /*if (ImGui::MenuItem("Show entity size"))
                {
                    auto v = scene.Serialize();
                }*/
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::InputFloat("Font Size Scale", &m_GuiFontScale, 0.1f, 0.3f))
                {
                    ImGui::GetIO().FontGlobalScale = m_GuiFontScale;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Create"))
            {
                if(ImGui::MenuItem("Entity"))
                {
                    CreateEmptyEntityInScene();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Log"); 
        ImGui::End();

        ImGui::Begin("Scene");
        auto it = SceneView<ecs::info_component>(m_Scene);
        for(auto const& entity_id : it)
        {
            auto* info = m_Scene.Get<ecs::info_component>(entity_id);
            if(info != nullptr)
            {
                char const* name = info->name;
                
                if (ImGui::Selectable(name, &info->isSelectedOnEditor))
                {
                    // Unselection
                    if (!info->isSelectedOnEditor)
                    {
                        m_SelectedEntity = INVALID_ENTITY;
                        continue;
                    }

                    // If we have selected an entity before, set the "selected"
                    // state of old selected entity to "unselected"
                    if(m_SelectedEntity != INVALID_ENTITY)
                    {
                        auto* ols_selected_entity_info = m_Scene.Get<ecs::info_component>(m_SelectedEntity);
                        ols_selected_entity_info->isSelectedOnEditor = false;
                    }
                    // Now we can set the new selected entity.
                    m_SelectedEntity = entity_id;
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Inspector");
        // Components
        if(ecs::IsEntityValid(m_SelectedEntity))
        {
            if (ImGui::BeginMenu("Add Component"))
            {
                if (ImGui::MenuItem("Transform"))
                {
                    m_Scene.Assign<ecs::transform_component>(m_SelectedEntity);
                }
                if (ImGui::MenuItem("Texture"))
                {
                    m_Scene.Assign<ecs::texture2d_component>(m_SelectedEntity);
                }

                ImGui::EndMenu();
            }
            DrawTransformComponent();
            DrawTexture2DComponent();
        }
        ImGui::End();

        // Drawing the game framebuffer into an imgui image
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Game");
        ImVec2 drawable_area_dimensions   = ImGui::GetContentRegionAvail();
        float  drawable_area_aspect_ratio = drawable_area_dimensions.x / drawable_area_dimensions.y;

        if (m_EditorFramebufferAspectRatioBefore != drawable_area_aspect_ratio)
        {

            if (m_AspectRatio >= drawable_area_aspect_ratio)
            {
                m_EditorFramebufferDimensions.x = drawable_area_dimensions.x;
                m_EditorFramebufferDimensions.y = drawable_area_dimensions.x / m_AspectRatio;
            }
            else
            {
                m_EditorFramebufferDimensions.x = drawable_area_dimensions.y * m_AspectRatio;
                m_EditorFramebufferDimensions.y = drawable_area_dimensions.y;
            }

            m_EditorFramebufferAspectRatioBefore = drawable_area_aspect_ratio;
        }
        
        auto framebuffer_texture_id = reinterpret_cast<ImTextureID>(static_cast<uint64>(m_EditorFramebuffer.GetFrameBufferTextureHandle()));
        
        
        ImGui::Image(
            framebuffer_texture_id,
            m_EditorFramebufferDimensions,
            m_EditorFramebufferUVCoords1,
            m_EditorFramebufferUVCoords2
        );
        ImGui::End();
        ImGui::PopStyleVar();
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

        m_Key |= current_key;
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
        
        m_Key &= ~current_key;
    }

    void Editor::OnMouseMove(mouse_move&)
    {
    }

    void Editor::CreateEmptyEntityInScene()
    {
        m_Scene.NewEntity();
    }

    void Editor::DrawTransformComponent()
    {
        bool has_transform = m_Scene.HasComponent<ecs::transform_component>(m_SelectedEntity);
        if (has_transform && ImGui::CollapsingHeader("Transform"))
        {
            auto* transform_component = m_Scene.Get<ecs::transform_component>(m_SelectedEntity);

            bool transform_changed{};
            transform_changed |= ImGui::DragFloat3("Position", glm::value_ptr(transform_component->position), 0.01f);
            transform_changed |= ImGui::DragFloat3("Scale", glm::value_ptr(transform_component->scale), 0.01f);
            if (transform_changed)
            {
                transform_component->position = shapes::MoveShape2DToPosition(m_Model, transform_component->position);
                m_ScaledModel                  = shapes::ScaleShape2D(m_Model, transform_component->scale);
            }
        }
    }

    void Editor::DrawTexture2DComponent()
    {
        bool has_texture2d = m_Scene.HasComponent<ecs::texture2d_component>(m_SelectedEntity);
        if (has_texture2d && ImGui::CollapsingHeader("Texture 2D"))
        {
            auto* texture2d_component = m_Scene.Get<ecs::texture2d_component>(m_SelectedEntity);
            ImGui::ImageButton(
                reinterpret_cast<ImTextureID>(static_cast<uint64>(texture2d_component->texture->id)),
                ImVec2{200.0f, 200.0f},
                // we reuse these to ensure correct rotation.
                m_EditorFramebufferUVCoords1,
                m_EditorFramebufferUVCoords2
            );

            ImGui::DragFloat2("UV Coords", glm::value_ptr(texture2d_component->uvCoordinates), 0.01f);
        }
    }

    Application* CreateApplication(void* p_PlacementPtr)
    {
        window_props props;
        props.title = "Hello";
        props.width = 1920;
        props.height = 1080;
        props.refreshRate = 120;
        props.vsync = true;
        props.fullscreen = false;

        auto a = WindowSystem::NewWindow(props);

        Application* app = p_PlacementPtr != nullptr ? new (p_PlacementPtr) Editor() : new Editor();
        return app;
    }
}