#include "polos/polos_pch.h"

#include <bitset>

#include <glad/glad.h>
#include <imgui/imgui_include.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "polos/core/update_queue.h"
#include "polos/context/shader_lib.h"
#include "polos/core/event_bus.h"
#include "polos/core/window_system.h"
#include "polos/renderer/renderer.h"
#include "polos/renderer/shapes/shapes2d_transform.h"
#include "polos/core/ecs/components/components.h"
#include "polos/core/scene/scene_view.h"

#include "editor.h"

#include <polos.h>

namespace polos
{
    Editor::Editor()
        : m_EditorCamera{{0.0f, 0.0f, 5.0f}, {0.0f, 1.0f, 0.0f}},
        m_GuiFontScale{1.8f},
        m_Key{0},
        m_Model{glm::mat4{1.0f}},
        m_EditorFramebufferAspectRatioBefore{0.0f},
        m_EditorFramebufferDimensions{ImVec2{0.0f, 0.0f}},
        m_EditorFramebufferUVCoords1{ImVec2{0.0f, 1.0f}},
        m_EditorFramebufferUVCoords2{ImVec2{1.0f, 0.0f}}
    {
        UPDATE_Q_MEM_ADD_LAST(Update);
        
        m_AppWindow     = WindowSystem::GetAppWindowGUID();

        ShaderLib::Load("resources/shaders/basic_color.vert", "resources/shaders/basic_color.frag");
        ShaderLib::Load("resources/shaders/texture.vert", "resources/shaders/texture.frag");
        m_ShaderTexture    = &ShaderLib::Get("texture"_sid);
        m_ShaderBasicColor = &ShaderLib::Get("basic_color"_sid);
        m_Texture = Texture::Load("resources/textures/linux-22621.png");

        auto l_AppWindowProps = WindowSystem::GetWindowProps(m_AppWindow);
        m_AppWindowWidth  = l_AppWindowProps->width;
        m_AppWindowHeight = l_AppWindowProps->height;
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

        auto* info = m_Scene.Assign<ecs::info_component>(m_TextureEntity);
        std::string name = "Texture";
        std::copy(name.begin(), name.end(), info->name);

        glm::vec3 const moveTo = {
            m_TextureEntityTransformComponent->position.x,
            m_TextureEntityTransformComponent->position.y,
            m_TextureEntityTransformComponent->position.z
        };

        // move to initial position
        shapes::MoveShape2DToPosition(m_Model, moveTo);
        shapes::ScaleShape2D(m_Model, m_TextureEntityTransformComponent->scale.x, m_TextureEntityTransformComponent->scale.y);

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
        static_cast<void>(p_DeltaTime);

        CameraMovement l_camera_move = kNone;

        if ((m_Key & (1)) != 0)
        {
            l_camera_move = kUp;
        }
        else if ((m_Key & (2)) != 0)
        {
            l_camera_move = kDown;
        }
        m_EditorCamera.ProcessKeyboard(l_camera_move, p_DeltaTime);
        if ((m_Key & (4)) != 0)
        {
            l_camera_move = kLeft;
        }
        else if ((m_Key & (8)) != 0)
        {
            l_camera_move = kRight;
        }
        m_EditorCamera.ProcessKeyboard(l_camera_move, p_DeltaTime);

        m_EditorFramebuffer.Clear();

        m_EditorFramebuffer.Bind();
        RenderTexture2D(m_Model, m_Texture, *m_ShaderTexture);
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
                const char* name = info->name;
                if (ImGui::Selectable(name, &info->isSelectedOnEditor, 0, ImVec2{100.0f, .0f}))
                {
                    if(!info->isSelectedOnEditor)
                    {
                        m_SelectedEntity = INVALID_ENTITY;
                        continue;
                    }
                    m_SelectedEntity = entity_id;
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Inspector");

        // Components
        if(ecs::IsEntityValid(m_SelectedEntity))
        {
            DrawTransformComponent();
            DrawTexture2DComponent();
        }
        ImGui::End();

        ImGui::Begin("Game");
        ImVec2 l_DrawableAreaDimensions = ImGui::GetContentRegionAvail();
        float  l_DrawableAreaAspectRatio = l_DrawableAreaDimensions.x / l_DrawableAreaDimensions.y;

        if (m_EditorFramebufferAspectRatioBefore != l_DrawableAreaAspectRatio)
        {
            if (m_AspectRatio >= l_DrawableAreaAspectRatio)
            {
                m_EditorFramebufferDimensions.x = l_DrawableAreaDimensions.x;
                m_EditorFramebufferDimensions.y = l_DrawableAreaDimensions.x / m_AspectRatio;
            }
            else
            {
                m_EditorFramebufferDimensions.x = l_DrawableAreaDimensions.y * m_AspectRatio;
                m_EditorFramebufferDimensions.y = l_DrawableAreaDimensions.y;
            }

            m_EditorFramebufferAspectRatioBefore = l_DrawableAreaAspectRatio;
        }
        
        auto l_FramebufferTextureId = reinterpret_cast<ImTextureID>(static_cast<uint64>(m_EditorFramebuffer.GetFrameBufferTextureHandle()));

        ImGui::Image(
            l_FramebufferTextureId,
            m_EditorFramebufferDimensions,
            m_EditorFramebufferUVCoords1,
            m_EditorFramebufferUVCoords2
        );
        ImGui::End();
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
        auto* l_TransformComponent = m_Scene.Get<ecs::transform_component>(m_SelectedEntity);
        if(ImGui::CollapsingHeader("Transform") && l_TransformComponent != nullptr)
        {
            bool l_PositionChange = false;
            l_PositionChange |= ImGui::DragFloat3("Position", glm::value_ptr(l_TransformComponent->position), 0.01f);
            glm::vec3 l_OldScale = l_TransformComponent->scale;
            l_PositionChange |= ImGui::DragFloat3("Scale", glm::value_ptr(l_TransformComponent->scale), 0.01f);
            glm::vec3 l_ScaleVector = glm::vec3(0.0f);
            if(l_OldScale.x == 0.0f && l_OldScale.y != 0.0f)
            {
                l_ScaleVector.x = l_TransformComponent->scale.x;
                l_ScaleVector.y = l_TransformComponent->scale.y / l_OldScale.y;
            }
            else if(l_OldScale.x != 0.0f && l_OldScale.y == 0.0f)
            {
                l_ScaleVector.x = l_TransformComponent->scale.x / l_OldScale.x;
                l_ScaleVector.y = l_TransformComponent->scale.y;
            }
            else if(l_OldScale.x == 0.0f && l_OldScale.y == 0.0f)
            {
                l_ScaleVector = l_TransformComponent->scale;
            }
            l_ScaleVector = l_TransformComponent->scale / l_OldScale;
            if (l_PositionChange)
            {
                l_TransformComponent->position = shapes::MoveShape2DToPosition(m_Model, l_TransformComponent->position);
                shapes::ScaleShape2D(m_Model, l_ScaleVector);
            }
        }
    }

    void Editor::DrawTexture2DComponent()
    {
        auto* l_Texture2dComponent = m_Scene.Get<ecs::texture2d_component>(m_SelectedEntity);
        if(ImGui::CollapsingHeader("Texture 2D") && l_Texture2dComponent != nullptr)
        {
            ImGui::ImageButton(
                reinterpret_cast<ImTextureID>(static_cast<uint64>(m_Texture->id)),
                ImVec2{200.0f, 200.0f},
                m_EditorFramebufferUVCoords1,
                m_EditorFramebufferUVCoords2
            );

            ImGui::DragFloat2("UV Coords", glm::value_ptr(l_Texture2dComponent->uvCoordinates), 0.01f);
        }
    }

    Application* CreateApplication(void* ptr)
    {
        window_props props;
        props.title = "Hello";
        props.width = 3840;
        props.height = 2160;
        props.refreshRate = 120;
        props.vsync = true;
        props.fullscreen = false;

        auto a = WindowSystem::NewWindow(props);

        Application* app = !ptr ? new Editor() : new (ptr) Editor();
        return app;
    }
}