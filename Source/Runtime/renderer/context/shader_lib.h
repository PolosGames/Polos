#pragma once
#ifndef POLOS_RENDERER_CONTEXT_SHADER_H_
#define POLOS_RENDERER_CONTEXT_SHADER_H_

#include <glm/glm.hpp>

#include "utils/alias.h"
#include "containers/containers.h"
#include "shader.h"

namespace polos
{
    class ShaderLib
    {
    public:
        void    Load(std::string_view glsl_file);
        void    Load(std::string_view vert_file, std::string_view frag_file);
        void    Finalize();

        Shader& Get(StringId shader_name);
        
        static ShaderLib& Instance()
        {
            return *m_Instance;
        }
    private:
        static uint32 compile_shader(std::string_view source, uint32 shader_type);
        static bool is_successful(uint32 id, uint32 action);
    private:
        friend class Engine;
        static ShaderLib* m_Instance;
        HashMap<StringId, Shader> m_Shaders;
    };
}

#endif /* POLOS_RENDERER_CONTEXT_SHADER_H_ */