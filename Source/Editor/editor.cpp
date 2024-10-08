
#include "editor.h"

#include <glad/glad.h>
#include <imgui/imgui_backend.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "polos/graphics/shader_lib.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"
#include "polos/graphics/renderer.h"
#include "polos/graphics/shapes/shapes2d_transform.h"
#include "polos/core/ecs/components/components.h"
#include "polos/core/scene/common_set_scene_view.h"
#include "polos/core/ecs/sets/info_set.h"
#include "polos/core/ecs/sets/camera_set.h"
#include "polos/core/resource/resource.h"

#include <polos.h>

namespace polos
{
    Editor::Editor()
        : m_GuiFontScale{1.8f}
        , m_EditorFramebufferAspectRatioBefore{0.0f}
        , m_EditorFramebufferUVCoords1{ImVec2{0.0f, 1.0f}}
        , m_EditorFramebufferUVCoords2{ImVec2{1.0f, 0.0f}}
    {
        SUB_TO_EVENT_MEM_FUN(engine_update, update);

        resource::LoadResource<resource::image>("resources/textures/linux-22621.png");

        m_AppWindow     = WindowSystem::GetAppWindowGUID();
        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        ShaderLib::Load("resources/shaders/texture.vert", "resources/shaders/texture.frag");
        m_ShaderTexture    = &ShaderLib::Get("texture"_sid);
        m_ShaderBasicColor = &ShaderLib::Get("basic_color"_sid);

        Optional<window_props> app_window_props = WindowSystem::GetWindowProps(m_AppWindow);
        m_AppWindowWidth  = app_window_props->width;
        m_AppWindowHeight = app_window_props->height;
        m_AspectRatio     = static_cast<float>(m_AppWindowWidth) / static_cast<float>(m_AppWindowHeight);

        m_EditorFramebuffer.Initialize(m_AppWindowWidth, m_AppWindowHeight);
        m_EditorFramebuffer.Bind();
        glViewport(0, 0, m_AppWindowWidth, m_AppWindowHeight);
        m_EditorFramebuffer.Unbind();

        ecs::Entity texture_entity = m_Scene.NewEntity();

        auto* entity_transform_component = m_Scene.Assign<ecs::transform_component>(texture_entity);
        auto* entity_texture_component   = m_Scene.Assign<ecs::texture2d_component>(texture_entity);
        auto* entity_material_component  = m_Scene.Assign<ecs::material_component>(texture_entity);

        entity_transform_component->position = glm::vec3(0.0f, 0.0f, 0.0f);
        entity_transform_component->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        entity_transform_component->scale    = glm::vec3(1.0f, 1.0f, 1.0f);

        entity_texture_component->texture = Texture::Load("linux-22621");

        entity_material_component->shader = m_ShaderTexture;

        auto* info_component = m_Scene.Get<ecs::info_component>(texture_entity);
        info_component->name = "Texture";

        // Add the editor camera
        m_EditorCamera = m_Scene.NewEntity();

        auto* editor_camera_transform_comp = m_Scene.Assign<ecs::transform_component>(m_EditorCamera);
        auto* editor_camera_camera_comp    = m_Scene.Assign<ecs::camera_component>(m_EditorCamera);
        auto* editor_camera_info_comp      = m_Scene.Assign<ecs::info_component>(m_EditorCamera);

        editor_camera_transform_comp->position = glm::vec3(0.0f, 0.0f, 5.0f);
        editor_camera_transform_comp->rotation = glm::vec3(0.0f, -90.0f, 0.0f);
        editor_camera_transform_comp->scale    = glm::vec3(1.0f, 1.0f, 1.0f);

        editor_camera_camera_comp->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        editor_camera_camera_comp->isEditor = true;
        editor_camera_camera_comp->type = CameraType::k_Ortho;

        editor_camera_info_comp->name = "Camera";

        ecs::camera_set::UpdateCameraVectors(editor_camera_transform_comp, editor_camera_camera_comp);

        SUB_TO_EVENT_MEM_FUN(mouse_move, OnMouseMove);
        SUB_TO_EVENT_MEM_FUN(key_press, OnKeyPress);
        SUB_TO_EVENT_MEM_FUN(key_release, OnKeyRelease);

        m_SelectedEntity = INVALID_ENTITY;

        CameraController::AttachScene(&m_Scene);
    }

    Editor::~Editor()
    {
    }

    void Editor::update(engine_update& p_Event)
    {
        m_EditorFramebuffer.Clear();

        m_EditorFramebuffer.Bind();
        Renderer::RenderScene(m_Scene);
        //RenderRectangle(m_Model, *m_ShaderBasicColor);
        m_EditorFramebuffer.Unbind();

        //ImGui::ShowDemoWindow();

        RenderGUI(p_Event.deltaTime);
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
        for(auto info : CommonSetSceneView<ecs::info_set>(m_Scene))
        {
            if(info.infoComponent != nullptr)
            {
                if (ImGui::Selectable(info.infoComponent->name.c_str(), &info.infoComponent->isSelectedOnEditor))
                {
                    // Unselection
                    if (!info.infoComponent->isSelectedOnEditor)
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
                    m_SelectedEntity = info.entity;
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

            ImGui::DragFloat3("Position", glm::value_ptr(transform_component->position), 0.01f);
            ImGui::DragFloat3("Scale", glm::value_ptr(transform_component->scale), 0.01f);
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
            texture2d_component->hasUvChanged |= ImGui::DragFloat2("UV Bottom Left", glm::value_ptr(texture2d_component->uvBottomLeft), 0.01f);
            texture2d_component->hasUvChanged |= ImGui::DragFloat2("UV Bottom Right", glm::value_ptr(texture2d_component->uvBottomRight), 0.01f);
            texture2d_component->hasUvChanged |= ImGui::DragFloat2("UV Top Right", glm::value_ptr(texture2d_component->uvTopRight), 0.01f);
            texture2d_component->hasUvChanged |= ImGui::DragFloat2("UV Top Left", glm::value_ptr(texture2d_component->uvTopLeft), 0.01f);
        }
    }

    Application* CreateApplication(void* p_PlacementPtr)
    {
        window_props props;
        props.title = "Polos Editor";
        props.width = 1920;
        props.height = 1080;
        props.refreshRate = 120;
        props.vsync = true;
        props.fullscreen = false;

        auto a = WindowSystem::NewWindow(props);
        static_cast<void>(a);

        Application* app = p_PlacementPtr != nullptr ? new (p_PlacementPtr) Editor() : new Editor();
        return app;
    }
}// namespace polos
