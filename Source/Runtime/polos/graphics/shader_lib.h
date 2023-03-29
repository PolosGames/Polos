#pragma once

#include <glm/glm.hpp>

#include "polos/utils/alias.h"
#include "polos/containers/containers.h"
#include "polos/graphics/shader.h"

namespace polos
{
    class ShaderLib
    {
    public:
        ShaderLib();
    public:
        void Startup();

        static void    Load(std::string_view p_GlslFile);
        static void    Load(std::string_view p_VertFile, std::string_view p_FragFile);

        static Shader&               Get(StringId p_ShaderNameSid);
        static DArray<Shader> const& GetAll();
    private:
        static ShaderLib* s_Instance;

        DArray<StringId>  m_ShaderNames;
        DArray<Shader>    m_Shaders;
    };
}
