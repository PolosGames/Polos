#include "polos/polos_pch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "polos/utils/stringid.h"

#include "polos/context/shader.h"

namespace polos
{
    Shader::Shader(uint32 program_id)
        : m_ProgramId(program_id)
    {
        CreateUniformLookup();
    }

    void Shader::CreateUniformLookup()
    {
        GLuint const uniform_count = [&program_id = m_ProgramId]()
        {
            GLint u_count = 0;
            glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &u_count);
            return static_cast<GLuint>(u_count);
        }();

        GLint max_name_len = 0;
        GLsizei length     = 0;
        GLsizei count      = 0;
        GLenum type        = GL_NONE;
        glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        auto uniform_name = std::make_unique<char[]>(static_cast<std::size_t>(max_name_len));

        for (GLuint i = 0; i < uniform_count; ++i)
        {
            glGetActiveUniform(m_ProgramId, i, max_name_len, &length, &count, &type, uniform_name.get());

            m_LookupTable.insert(
                {
                    get_string_id(uniform_name.get()),
                    { glGetUniformLocation(m_ProgramId, uniform_name.get()), count }
                }
            );
        }
    }
    
    void Shader::SetInt(StringId name, int32 value)
    {
        glUniform1i(m_LookupTable.at(name).location, value);
    }

    void Shader::SetFloat(StringId name, float value)
    {
        glUniform1f(m_LookupTable.at(name).location, value);
    }

    void Shader::SetDouble(StringId name, double value)
    {
        glUniform1d(m_LookupTable.at(name).location, value);
    }

    void Shader::SetUint(StringId name, uint32 value)
    {
        glUniform1ui(m_LookupTable.at(name).location, value);
    }

    void Shader::SetMat(StringId name, glm::mat4 const& value)
    {
        glUniformMatrix4fv(m_LookupTable.at(name).location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetVec(StringId name, glm::vec2 const& value)
    {
        glUniform2fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetVec(StringId name, glm::vec3 const& value)
    {
        glUniform3fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetVec(StringId name, glm::vec4 const& value)
    {
        glUniform4fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }
    
    void Shader::Use() const
    {
        glUseProgram(m_ProgramId);
    }
} // namespace polos

#endif /* USE_OPENGL */