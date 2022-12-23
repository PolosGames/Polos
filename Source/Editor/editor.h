#ifndef POLOS_EDITOR_H_
#define POLOS_EDITOR_H_

#include "polos/core/application.h"
#include "polos/context/shader.h"
#include "polos/context/vao.h"
#include "polos/core/window.h"

namespace polos
{   
    class Editor : public Application
    {
    public:
        Editor();
        ~Editor();
    private:
        void Update(float delta_time);
    private:
        Shader basic_color;

        Vao cube;
        
        glm::mat4 model;
        glm::mat4 view, projection;
        glm::vec3 pos;
        glm::vec3 slider_pos;

        polos::GUID app_window;
        polos::GUID popup_window;

        bool open;
    };
}

#endif // POLOS_EDITOR_H_