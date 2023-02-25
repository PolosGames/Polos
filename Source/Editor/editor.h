#pragma once

#include "polos/core/application.h"
#include "polos/context/shader.h"
#include "polos/context/vao.h"
#include "polos/core/window.h"
#include "polos/core/camera.h"
#include "polos/events/input/mouse_move.h"
#include "polos/events/input/key_press.h"
#include "polos/events/input/key_release.h"
#include "polos/context/framebuffer.h"
#include "polos/core/scene/scene.h"

namespace polos
{   
    class Editor : public Application
    {
    public:
        Editor();
        ~Editor() override;
    private:
        void Update(float p_DeltaTime);
        void RenderGUI(float p_DeltaTime);

        void CreateEmptyEntityInScene();
        
        void OnKeyPress(key_press& e);
        void OnKeyRelease(key_release& e);
        void OnMouseMove(mouse_move& e);

        void DrawTransformComponent();
        void DrawTexture2DComponent();
    private:
        Shader* m_ShaderTexture;
        Shader* m_ShaderBasicColor;
        TextureRef m_Texture;
        
        glm::mat4 m_Model;

        int32 m_Key;

        polos::GUID m_AppWindow;
        int32 m_AppWindowWidth;
        int32 m_AppWindowHeight;
        float m_AspectRatio;

        float m_GuiFontScale;

        pl::Camera m_EditorCamera;
        FrameBuffer m_EditorFramebuffer;
        float  m_EditorFramebufferAspectRatioBefore;
        ImVec2 m_EditorFramebufferDimensions;
        ImVec2 m_EditorFramebufferUVCoords1;
        ImVec2 m_EditorFramebufferUVCoords2;

        Scene m_Scene;
        ecs::Entity m_SelectedEntity;
        ecs::Entity m_TextureEntity;
        ecs::transform_component* m_TextureEntityTransformComponent;
        ecs::texture2d_component* m_TextureEntityTexture2DComponent;
    };
}
