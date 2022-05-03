#ifndef POLOS_EDITOR_H_
#define POLOS_EDITOR_H_

#include "core/application.h"
#include "renderer/context/shader.h"
#include "renderer/context/vao.h"
#include "core/i_window.h"

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
        
        glm::mat4 model, model2;
        glm::mat4 view, projection;
        glm::vec3 pos, pos2;
        glm::vec3 slider_pos, slider_pos2;

        bool open;
    };
}

#endif // POLOS_EDITOR_H_