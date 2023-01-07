#ifndef POLOS_EDITOR_H_
#define POLOS_EDITOR_H_

#include "polos/core/application.h"
#include "polos/context/shader.h"
#include "polos/context/vao.h"
#include "polos/core/window.h"
#include "polos/core/camera.h"
#include "polos/events/input/mouse_move.h"
#include "polos/events/input/key_press.h"
#include "polos/events/input/key_release.h"

namespace polos
{   
    class Editor : public Application
    {
    public:
        Editor();
        ~Editor();
    private:
        void Update(float delta_time);
        
        void OnKeyPress(key_press& e);
        void OnKeyRelease(key_release& e);
        void OnMouseMove(mouse_move& e);
    private:
        Shader* m_TextureShader;
        TextureRef texture;

        pl::Camera camera;
        
        glm::mat4 model;

        int32 key;

        polos::GUID app_window;
    };
}

#endif // POLOS_EDITOR_H_