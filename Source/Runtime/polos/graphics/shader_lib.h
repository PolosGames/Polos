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
        static void    Load(std::string_view p_GlslFile);
        static void    Load(std::string_view p_VertFile, std::string_view p_FragFile);
        static void    Finalize();

        static Shader& Get(StringId p_ShaderNameSid);
        
    private:
        static uint32 compile_shader(std::string_view p_Source, uint32 p_ShaderType);
        static bool   is_successful(uint32 p_Id, uint32 p_Action);
    private:
        friend class Engine;
        static ShaderLib* s_Instance;
        HashMap<StringId, Shader> m_Shaders;
    };
}
