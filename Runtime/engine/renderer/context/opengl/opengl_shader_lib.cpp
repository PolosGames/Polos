#include "polos_pch.h"

#ifdef USE_OPENGL

#include <glad/glad.h>

#include "io/file.h"
#include "utils/stringid.h"
#include "utils/alias.h"
#include "debug/plassert.h"

#include "context/shader_lib.h"

namespace polos
{
    ShaderLib* ShaderLib::m_Instance;

    void ShaderLib::Load(std::string_view glsl_file)
    {
        StringId shader_name;
        std::string const shader_code = [&glsl_file, &shader_name] {
            File file(glsl_file.data(), kRead);
            shader_name = get_string_id(file.file_name);
            return file.Read();
        }();
        
        if(m_Shaders.contains(shader_name)) return;
        
        int32 i = 0; // how many shaders are created
        uint32 shader_ids[kShaderTypeMax];
        std::size_t pos = 0;
        for(;pos != std::string::npos; pos = shader_code.find_first_of("#shader", pos), i++)
        {
            std::size_t eol = shader_code.find_first_of('\n', pos);
            std::size_t type_start = pos + 8;
            ASSERT(eol != std::string::npos);
            StringId s_shader_type = get_string_id(shader_code.substr(type_start, eol - type_start));
            
            std::size_t start_of_shader = shader_code.find_first_of("#version", eol);
            std::size_t end_of_shader   = shader_code.find("#shader", eol);
            pos = end_of_shader;
            std::string shader_source   = shader_code.substr(start_of_shader, end_of_shader - start_of_shader);
            
            uint32 const shader_type = [s_shader_type] {
                switch (s_shader_type)
                {
                    case "vertex"_sid:   return GL_VERTEX_SHADER;
                    case "fragment"_sid: return GL_FRAGMENT_SHADER;
                    case "geometry"_sid: return GL_GEOMETRY_SHADER;
                    case "compute"_sid:  return GL_COMPUTE_SHADER;
                    default: LOG_CORE_ERROR("Shader type not recognized"); return 0;
                }
            }();
            shader_ids[i] = compile_shader(shader_source, shader_type);
        }
        
        uint32 program_id = glCreateProgram();
        for(int k = 0; k < i; k++)
        {
            glAttachShader(program_id, shader_ids[k]);
        }
        glLinkProgram(program_id);

        if (!is_successful(program_id, GL_LINK_STATUS)) return;
        
        m_Shaders.insert({shader_name, Shader{program_id}});

        //glDeleteShader(vertex_shader);
        //glDeleteShader(fragment_shader);
    }
    
    Shader& ShaderLib::Get(StringId shader_name)
    {
        if(!m_Shaders.contains(shader_name)) LOG_CORE_WARN(R"(SHADER "{}" DOESN'T EXIST!)", shader_name);
        Shader& s = m_Shaders[shader_name];
        return s;
    }
    
    Shader& ShaderLib::Get(cstring shader_name)
    {
        StringId sid = get_string_id(shader_name);
        if(!m_Shaders.contains(sid)) LOG_CORE_WARN(R"(SHADER "{}" DOESN'T EXIST!)", shader_name);
        Shader& s = m_Shaders[sid];
        return s;
    }
    
    uint32 ShaderLib::compile_shader(std::string_view source, uint32 shader_type)
    {
        cstring shader_source = source.data();
        uint32 shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &shader_source, nullptr);
        glCompileShader(shader_id);
        
        if (!is_successful(shader_id, GL_COMPILE_STATUS)) return uint32(-1);
        
        return shader_id;
    }

    bool ShaderLib::is_successful(uint32 id, uint32 action)
    {
        int32 success = -1;
        char info_log[512];
        if (action == GL_COMPILE_STATUS)
        {
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(id, 512, nullptr, info_log);
                LOG_CORE_ERROR("Shader Compilation Error: {0}", info_log);
            }
        }
        else if (action == GL_LINK_STATUS)
        {
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(id, 512, nullptr, info_log);
                LOG_CORE_ERROR("Shader Linking Error: ", info_log);
            }
        }

        return !!success;
    }
} // namespace polos

#endif // USE_OPENGL
