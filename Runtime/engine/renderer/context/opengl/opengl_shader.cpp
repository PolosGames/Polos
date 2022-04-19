#include "polos_pch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/context/shader.h"

namespace polos
{
    Shader::Shader(uint32 program_id)
        : m_ProgramId(program_id)
    {}
    
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