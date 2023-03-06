
#if defined(USE_OPENGL)

#include "polos/graphics/shader_lib.h"

#include <glad/glad.h>

#include "polos/core/io/file.h"
#include "polos/utils/stringid.h"

namespace polos
{
    ShaderLib* ShaderLib::s_Instance;

    void ShaderLib::Load(std::string_view p_GlslFile)
    {
        StringId shader_name;
        std::string const shader_code = [&p_GlslFile, &shader_name] {
            File file(p_GlslFile.data(), k_Read);
            shader_name = get_string_id(file.fileName);
            return file.ReadStr();
        }();
        
        if(s_Instance->m_Shaders.contains(shader_name)) return;
        
        int32 i = 0; // how many shaders are created
        uint32 shader_ids[kShaderTypeMax];
        std::size_t pos = 0;
        for(; pos != std::string::npos; pos = shader_code.find_first_of("#shader", pos), i++)
        {
            std::size_t eol = shader_code.find_first_of('\n', pos);
            std::size_t type_start = pos + 8;
            ASSERT(eol != std::string::npos);
            StringId s_shader_type = get_string_id(shader_code.substr(type_start, eol - type_start));
            
            std::size_t start_of_shader = shader_code.find_first_of("#version", eol);
            std::size_t end_of_shader   = shader_code.find("#shader", eol);
            pos = end_of_shader;
            std::string shader_source   = shader_code.substr(start_of_shader, end_of_shader - start_of_shader);
            
            uint32 const shader_type = [s_shader_type]() -> uint32 {
                switch (s_shader_type)
                {
                    case "vertex"_sid:   return GL_VERTEX_SHADER;
                    case "fragment"_sid: return GL_FRAGMENT_SHADER;
                    case "geometry"_sid: return GL_GEOMETRY_SHADER;
                    case "compute"_sid:  return GL_COMPUTE_SHADER;
                    default: LOG_ENGINE_ERROR("Shader type not recognized"); return 0;
                }
            }();
            shader_ids[i] = compile_shader(shader_source.c_str(), shader_type);
        }
        
        uint32 program_id = glCreateProgram();
        for (int k = 0; k < i; ++k)
        {
            glAttachShader(program_id, shader_ids[k]);
        }
        glLinkProgram(program_id);

        if (!is_successful(program_id, GL_LINK_STATUS)) return;
        
        s_Instance->m_Shaders.insert({shader_name, Shader{program_id}});
    }

    void ShaderLib::Load(std::string_view p_VertFile, std::string_view p_FragFile)
    {
        StringId shader_name;
        std::string const vert_code = [&p_VertFile, &shader_name] {
            File file(p_VertFile.data(), k_Read);
            shader_name = get_string_id(file.fileName);
            return file.ReadStr();
        }();

        if (s_Instance->m_Shaders.contains(shader_name)) return;

        std::string const frag_code = [&p_FragFile] {
            File file(p_FragFile.data(), k_Read);
            return file.ReadStr();
        }();

        auto vert_id = compile_shader(vert_code.c_str(), GL_VERTEX_SHADER);
        auto frag_id = compile_shader(frag_code.c_str(), GL_FRAGMENT_SHADER);

        uint32 program_id = glCreateProgram();
        glAttachShader(program_id, vert_id);
        glAttachShader(program_id, frag_id);

        glLinkProgram(program_id);

        if (!is_successful(program_id, GL_LINK_STATUS)) return;

        s_Instance->m_Shaders.insert({shader_name, Shader{program_id}});
    }
    
    Shader& ShaderLib::Get(StringId p_ShaderNameSid)
    {
        if (!s_Instance->m_Shaders.contains(p_ShaderNameSid))
        {
            LOG_ENGINE_WARN(R"(SHADER "{}" DOESN'T EXIST!)", p_ShaderNameSid); 
        }
        return s_Instance->m_Shaders[p_ShaderNameSid];
    }
    
    uint32 ShaderLib::compile_shader(std::string_view p_Source, uint32 p_ShaderType)
    {
        cstring shader_source = p_Source.data();
        uint32 shader_id = glCreateShader(p_ShaderType);
        glShaderSource(shader_id, 1, &shader_source, nullptr);
        glCompileShader(shader_id);
        
        if (!is_successful(shader_id, GL_COMPILE_STATUS)) return static_cast<uint32>(-1);
        
        return shader_id;
    }

    bool ShaderLib::is_successful(uint32 p_Id, uint32 p_Action)
    {
        int32 success = -1;
        char info_log[512];
        if (p_Action == GL_COMPILE_STATUS)
        {
            glGetShaderiv(p_Id, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(p_Id, 512, nullptr, info_log);
                LOG_ENGINE_ERROR("Shader Compilation Error: {0}", info_log);
            }
        }
        else if (p_Action == GL_LINK_STATUS)
        {
            glGetProgramiv(p_Id, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(p_Id, 512, nullptr, info_log);
                LOG_ENGINE_ERROR("Shader Linking Error: ", info_log);
            }
        }

        return !!success;
    }
} // namespace polos

#endif // USE_OPENGL
