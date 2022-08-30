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
        SharedPtr<Window> win;
        
        glm::mat4 model, model2;
        glm::mat4 view, projection;
        glm::vec3 pos, pos2;
        glm::vec3 slider_pos, slider_pos2;

        bool open;
    };
}

#endif // POLOS_EDITOR_H_