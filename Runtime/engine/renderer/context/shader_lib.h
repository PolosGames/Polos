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
        void Load(std::string_view glsl_file);
        Shader& Get(std::string const& shader_name);
    private:
        static uint32 compile_shader(std::string_view source, uint32 shader_type);
        static bool is_successful(uint32 id, uint32 action);
    private:
        // TODO: Test if getuniformlocation is faster than creating a lookup table
        HashMap<StringId, uint32> m_UniformLookup;
        HashMap<StringId, Shader> m_Shaders;
    };
}

#endif /* POLOS_RENDERER_CONTEXT_SHADER_H_ */