#include "polos_pch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/context/shader.h"

namespace polos
{
    namespace detail
    {
        struct uniform_info
        {
            GLint   location;
            GLsizei count;
        };
    }

    Shader::Shader(uint32 program_id)
        : m_ProgramId(program_id)
    {}

    void Shader::CreateUniformLookup()
    {
        GLint uniform_count = 0;
        glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORMS, &uniform_count);

        if (uniform_count != 0)
        {
            GLint max_name_len = 0;
            GLsizei length     = 0;
            GLsizei count      = 0;
            GLenum type        = GL_NONE;
            glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

            char* uniform_name = static_cast<char*>(alloca(max_name_len));

            std::unordered_map<std::string, detail::uniform_info> uniforms;

            for (GLint i = 0; i < uniform_count; ++i)
            {
                glGetActiveUniform(m_ProgramId, i, max_name_len, &length, &count, &type, uniform_name);

                detail::uniform_info uniform_info = {};
                uniform_info.location             = glGetUniformLocation(m_ProgramId, uniform_name);
                uniform_info.count                = count;

                uniforms.emplace(std::make_pair(std::string(uniform_name, length), uniform_info));
            }
        }
    }
    
    void Shader::SetInt(cstring name, int32 value)
    {
        glUniform1i(glGetUniformLocation(m_ProgramId, name), value);
    }
    
    void Shader::SetFloat(cstring name, float value)
    {
        glUniform1f(glGetUniformLocation(m_ProgramId, name), value);
    }
    
    void Shader::SetDouble(cstring name, double value)
    {
        glUniform1d(glGetUniformLocation(m_ProgramId, name), value);
    }
    
    void Shader::SetUint(cstring name, uint32 value)
    {
        glUniform1ui(glGetUniformLocation(m_ProgramId, name), value);
    }
    
    void Shader::SetMat(cstring name, glm::mat4 const& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, name), 1, GL_FALSE, glm::value_ptr(value));
    }
    
    void Shader::SetVec(cstring name, glm::vec2 const& value)
    {
        glUniform2fv(glGetUniformLocation(m_ProgramId, name), 1, glm::value_ptr(value));
    }
    
    void Shader::SetVec(cstring name, glm::vec3 const& value)
    {
        glUniform3fv(glGetUniformLocation(m_ProgramId, name), 1, glm::value_ptr(value));
    }
    
    void Shader::SetVec(cstring name, glm::vec4 const& value)
    {
        glUniform4fv(glGetUniformLocation(m_ProgramId, name), 1, glm::value_ptr(value));
    }
    
    void Shader::Use() const
    {
        glUseProgram(m_ProgramId);
    }
} // namespace polos

#endif /* USE_OPENGL */