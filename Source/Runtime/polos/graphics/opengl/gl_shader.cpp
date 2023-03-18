#if defined(USE_OPENGL)

#include "polos/graphics/shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "polos/utils/stringid.h"

namespace polos
{

    Shader::Shader(uint32 m_ProgramId)
        : m_ProgramId(m_ProgramId)
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


        GLint   max_name_len{};
        GLsizei length{};
        GLsizei count{};
        GLenum  type{GL_NONE};
        glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        auto uniform_name = std::make_unique<char[]>(static_cast<std::size_t>(max_name_len));

        for (GLuint i = 0; i < uniform_count; ++i)
        {
            glGetActiveUniform(m_ProgramId, i, max_name_len, &length, &count, &type, uniform_name.get());

            m_LookupTable.insert(
                {
                    GetStringId(uniform_name.get()),
                    { glGetUniformLocation(m_ProgramId, uniform_name.get()), count }
                }
            );
        }
    }
    
    void Shader::Use() const
    {
        glUseProgram(m_ProgramId);
    }

    void Shader::Release() const
    {
        glUseProgram(0);
    }

#pragma region UniformSetters

#pragma region INT32
    // ========  INT32  ========

    void Shader::SetUniform(StringId const p_Name, int32 const p_Value) const
    {
        glUniform1i(m_LookupTable.at(p_Name).location, p_Value);
    }

    void Shader::SetUniform(StringId const p_Name, glm::i32vec2 const& p_Value) const
    {
        glUniform2iv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::i32vec3 const& p_Value) const
    {
        glUniform3iv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::i32vec4 const& p_Value) const
    {
        glUniform4iv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, std::span<int32> p_Value) const
    {
        int32 size = static_cast<int32>(p_Value.size());
        switch (size)
        {
            case 2:
                glUniform2iv(m_LookupTable.at(p_Name).location, 2, p_Value.data());
                break;
            case 3:
                glUniform3iv(m_LookupTable.at(p_Name).location, 3, p_Value.data());
                break;
            case 4:
                glUniform4iv(m_LookupTable.at(p_Name).location, 4, p_Value.data());
                break;
            default:
                LOG_ENGINE_WARN("The size of the int32 array that's being passed to shader must be in range [2,4].");
                break;
        }
    }
    // ======== ! INT32 ! ========

    // ========  UINT32  ========
    void Shader::SetUniform(StringId const p_Name, uint32 const p_Value) const
    {
        glUniform1ui(m_LookupTable.at(p_Name).location, p_Value);
    }

    void Shader::SetUniform(StringId const p_Name, glm::u32vec2 const& p_Value) const
    {
        glUniform2uiv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::u32vec3 const& p_Value) const
    {
        glUniform3uiv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId p_Name, glm::u32vec4 const& p_Value) const
    {
        glUniform4uiv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId p_Name, std::span<uint32> p_Value) const
    {
        int32 size = static_cast<int32>(p_Value.size());
        switch (size)
        {
            case 2:
                glUniform2uiv(m_LookupTable.at(p_Name).location, 2, p_Value.data());
                break;
            case 3:
                glUniform3uiv(m_LookupTable.at(p_Name).location, 3, p_Value.data());
                break;
            case 4:
                glUniform4uiv(m_LookupTable.at(p_Name).location, 4, p_Value.data());
                break;
            default:
                LOG_ENGINE_WARN("The size of the uint32 array that's being passed to shader must be in range [2,4].");
                break;
        }
    }
    /// ======= ! UINT32 ! =======

    //  ========  FLOAT  ========
    void Shader::SetUniform(StringId const p_Name, float const p_Value) const
    {
        glUniform1f(m_LookupTable.at(p_Name).location, p_Value);
    }

    void Shader::SetUniform(StringId const p_Name, glm::vec2 const& p_Value) const
    {
        glUniform2fv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::vec3 const& p_Value) const
    {
        glUniform3fv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::vec4 const& p_Value) const
    {
        glUniform4fv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, std::span<float> p_Value) const
    {
        int32 size  = static_cast<int32>(p_Value.size());
        float* data = p_Value.data();

        switch (size)
        {
            case 2:
                glUniform2fv(m_LookupTable.at(p_Name).location, 2, data);
                break;
            case 3:
                glUniform3fv(m_LookupTable.at(p_Name).location, 3, data);
                break;
            case 4:
                glUniform4fv(m_LookupTable.at(p_Name).location, 4, data);
                break;
            default:
                LOG_ENGINE_WARN("The size of the array that's being passed to shader must be in range [2,4].");
                break;
        }
    }
    // ======== ! FLOAT ! ========

    // ========  DOUBLE  ========
    void Shader::SetUniform(StringId const p_Name, double const p_Value) const
    {
        glUniform1d(m_LookupTable.at(p_Name).location, p_Value);
    }

    void Shader::SetUniform(StringId const p_Name, glm::dvec2 const& p_Value) const
    {
        glUniform2dv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::dvec3 const& p_Value) const
    {
        glUniform3dv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::dvec4 const& p_Value) const
    {
        glUniform3dv(m_LookupTable.at(p_Name).location, 1, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, std::span<double> p_Value) const
    {
        int32 size   = static_cast<int32>(p_Value.size());
        double* data = p_Value.data();

        switch (size)
        {
            case 2:
                glUniform2dv(m_LookupTable.at(p_Name).location, 2, data);
                break;
            case 3:
                glUniform3dv(m_LookupTable.at(p_Name).location, 3, data);
                break;
            case 4:
                glUniform4dv(m_LookupTable.at(p_Name).location, 4, data);
                break;
            default:
                LOG_ENGINE_WARN("The size of the array that's being passed to shader must be in range [2,4].");
                break;
        }
    }
    // ======== ! DOUBLE ! ========

    void Shader::SetUniform(StringId const p_Name, glm::mat2 const& p_Value) const
    {
        glUniformMatrix2fv(m_LookupTable.at(p_Name).location, 1, GL_FALSE, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::mat3 const& p_Value) const
    {
        glUniformMatrix3fv(m_LookupTable.at(p_Name).location, 1, GL_FALSE, glm::value_ptr(p_Value));
    }

    void Shader::SetUniform(StringId const p_Name, glm::mat4 const& p_Value) const
    {
        glUniformMatrix4fv(m_LookupTable.at(p_Name).location, 1, GL_FALSE, glm::value_ptr(p_Value));
    }

} // namespace polos

#endif /* USE_OPENGL */