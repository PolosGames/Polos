#pragma once

#include <glm/glm.hpp>

#include "polos/utils/alias.h"
#include "polos/containers/containers.h"
#include "polos/context/shader.h"

namespace polos
{
    class ShaderLib
    {
    public:
        static void    Load(std::string_view glsl_file);
        static void    Load(std::string_view vert_file, std::string_view frag_file);
        static void    Finalize();

        static Shader& Get(StringId shader_name);
        
    private:
        static uint32 compile_shader(std::string_view source, uint32 shader_type);
        static bool is_successful(uint32 id, uint32 action);
    private:
        friend class Engine;
        static ShaderLib* m_Instance;
        HashMap<StringId, Shader> m_Shaders;
    };
}
