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
    
    void Shader::Use() const
    {
        glUseProgram(m_ProgramId);
    }

#pragma region INT32
    // ========  INT32  ========

    void Shader::SetUniform(StringId name, int32 value)
    {
        glUniform1i(m_LookupTable.at(name).location, value);
    }

    void Shader::SetUniform(StringId name, glm::i32vec2 const& value)
    {
        glUniform2iv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::i32vec3 const& value)
    {
        glUniform3iv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::i32vec4 const& value)
    {
        glUniform4iv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, std::span<int32> value)
    {
        int32 size = static_cast<int32>(value.size());
        switch (size)
        {
            case 2:
                glUniform2iv(m_LookupTable.at(name).location, 2, value.data());
                break;
            case 3:
                glUniform3iv(m_LookupTable.at(name).location, 3, value.data());
                break;
            case 4:
                glUniform4iv(m_LookupTable.at(name).location, 4, value.data());
                break;
            default:
                LOG_ENGINE_WARN("The size of the int32 array that's being passed to shader must be in range [2,4].");
                break;
        }
    }

    // ======== ! INT32 ! ========
#pragma endregion

#pragma region UINT32
    // ========  UINT32  ========

    void Shader::SetUniform(StringId name, uint32 value)
    {
        glUniform1ui(m_LookupTable.at(name).location, value);
    }

    void Shader::SetUniform(StringId name, glm::u32vec2 const& value)
    {
        glUniform2uiv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::u32vec3 const& value)
    {
        glUniform3uiv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::u32vec4 const& value)
    {
        glUniform4uiv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, std::span<uint32> value)
    {
        int32 size = static_cast<int32>(value.size());
        switch (size)
        {
            case 2:
                glUniform2uiv(m_LookupTable.at(name).location, 2, value.data());
                break;
            case 3:
                glUniform3uiv(m_LookupTable.at(name).location, 3, value.data());
                break;
            case 4:
                glUniform4uiv(m_LookupTable.at(name).location, 4, value.data());
                break;
            default:
                LOG_ENGINE_WARN("The size of the uint32 array that's being passed to shader must be in range [2,4].");
                break;
        }
    }

    /// ======= ! UINT32 ! =======
#pragma endregion

#pragma region FLOAT  
    // ========  FLOAT  ========

    void Shader::SetUniform(StringId name, float value)
    {
        glUniform1f(m_LookupTable.at(name).location, value);
    }

    void Shader::SetUniform(StringId name, glm::vec2 const& value)
    {
        glUniform2fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::vec3 const& value)
    {
        glUniform3fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::vec4 const& value)
    {
        glUniform4fv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, std::span<float> value)
    {
        int32  size = static_cast<int32>(value.size());
        float* data = value.data();

        switch (size)
        {
            case 2:
                glUniform2fv(m_LookupTable.at(name).location, 2, data);
                break;
            case 3:
                glUniform3fv(m_LookupTable.at(name).location, 3, data);
                break;
            case 4:
                glUniform4fv(m_LookupTable.at(name).location, 4, data);
                break;
            default:
                LOG_ENGINE_WARN("The size of the array that's being passed to shader must be in range [2,4].");
                break;
        }
    }

    // ======== ! FLOAT ! ========
#pragma endregion

#pragma region DOUBLE
    // ========  DOUBLE  ========

    void Shader::SetUniform(StringId name, double value)
    {
        glUniform1d(m_LookupTable.at(name).location, value);
    }

    void Shader::SetUniform(StringId name, glm::dvec2 const& value)
    {
        glUniform2dv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::dvec3 const& value)
    {
        glUniform3dv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::dvec4 const& value)
    {
        glUniform3dv(m_LookupTable.at(name).location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, std::span<double> value)
    {
        int32 size   = static_cast<int32>(value.size());
        double* data = value.data();

        switch (size)
        {
            case 2:
                glUniform2dv(m_LookupTable.at(name).location, 2, data);
                break;
            case 3:
                glUniform3dv(m_LookupTable.at(name).location, 3, data);
                break;
            case 4:
                glUniform4dv(m_LookupTable.at(name).location, 4, data);
                break;
            default:
                LOG_ENGINE_WARN("The size of the array that's being passed to shader must be in range [2,4].");
                break;
        }
    }

    // ======== ! DOUBLE ! ========
#pragma endregion

#pragma region MATRIX
    
    void Shader::SetUniform(StringId name, glm::mat2 const& value)
    {
        glUniformMatrix2fv(m_LookupTable.at(name).location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::mat3 const& value)
    {
        glUniformMatrix3fv(m_LookupTable.at(name).location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(StringId name, glm::mat4 const& value)
    {
        glUniformMatrix4fv(m_LookupTable.at(name).location, 1, GL_FALSE, glm::value_ptr(value));
    }
#pragma endregion

} // namespace polos

#endif /* USE_OPENGL */