#pragma once

#include <imgui/imgui_backend.h>

#include "polos/core/application.h"
#include "polos/graphics/shader.h"
#include "polos/graphics/vao.h"
#include "polos/core/window.h"
#include "polos/core/events/input/mouse_move.h"
#include "polos/core/events/input/key_press.h"
#include "polos/core/events/input/key_release.h"
#include "polos/graphics/framebuffer.h"
#include "polos/core/scene/scene.h"
#include "polos/utils/guid.h"

namespace polos
{   
    class Editor : public Application
    {
    public:
        Editor();
        ~Editor() override;
    private:
        void RenderGUI(float p_DeltaTime);

        void CreateEmptyEntityInScene();
        
        void OnKeyPress(key_press& e);
        void OnKeyRelease(key_release& e);
        void OnMouseMove(mouse_move& e);

        void DrawTransformComponent();
        void DrawTexture2DComponent();
    private:
        void update(engine_update& p_Event);
    private:
        Shader* m_ShaderTexture{};
        Shader* m_ShaderBasicColor{};

        int32 m_Key{};

        polos::GUID m_AppWindow;
        int32 m_AppWindowWidth{};
        int32 m_AppWindowHeight{};
        float m_AspectRatio{};

        float m_GuiFontScale{};

        ecs::Entity m_EditorCamera;
        FrameBuffer m_EditorFramebuffer;
        float  m_EditorFramebufferAspectRatioBefore{};
        ImVec2 m_EditorFramebufferDimensions{};
        ImVec2 m_EditorFramebufferUVCoords1{};
        ImVec2 m_EditorFramebufferUVCoords2{};

        Scene m_Scene;
        ecs::Entity m_SelectedEntity{};
    };
}
