#include "plpch.h"

#ifdef USE_OPENGL

#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include "renderer/context/shader.h"

namespace polos
{
	Shader::Shader(cstring vert_file, cstring frag_file)
	{
		Load(vert_file, frag_file);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ProgramId);
	}

	void Shader::Load(cstring vert_file, cstring frag_file)
	{
		std::string vertex_code;
		std::string fragment_code;

		std::ifstream vertex_shader_file{};
		std::ifstream fragment_shader_file{};

		{
			vertex_shader_file.open(vert_file);
			ASSERTSTR(!vertex_shader_file.fail(), "Vertex Shader file could not be opened.");

			fragment_shader_file.open(frag_file);
			ASSERTSTR(!fragment_shader_file.fail(), "Vertex Shader file could not be opened.");

			std::stringstream vertex_shader_stream, fragment_shader_stream;

			// read file's buffer contents into streams
			vertex_shader_stream << vertex_shader_file.rdbuf();
			fragment_shader_stream << fragment_shader_file.rdbuf();

			vertex_shader_file.close();
			fragment_shader_file.close();

			// convert stream into string
			vertex_code = vertex_shader_stream.str();
			fragment_code = fragment_shader_stream.str();
		}

		cstring vertex_src = vertex_code.c_str();
		cstring fragment_src = fragment_code.c_str();

		GLuint vertex_shader;
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_src, nullptr);
		glCompileShader(vertex_shader);

		if (!is_successful(vertex_shader, GL_COMPILE_STATUS)) return;

		GLuint fragment_shader;
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_src, nullptr);
		glCompileShader(fragment_shader);

		if (!is_successful(fragment_shader, GL_COMPILE_STATUS)) return;

		m_ProgramId = glCreateProgram();
		glAttachShader(m_ProgramId, vertex_shader);
		glAttachShader(m_ProgramId, fragment_shader);
		glLinkProgram(m_ProgramId);

		if (!is_successful(m_ProgramId, GL_LINK_STATUS)) return;

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
	}

	void Shader::Use() const
	{
		glUseProgram(m_ProgramId);
	}

	bool Shader::is_successful(uint32 id, uint32 action)
	{
		int32 success = -1;
		char info_log[512];
		if (action == GL_COMPILE_STATUS)
		{
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(id, 512, NULL, info_log);
				LOG_CORE_ERROR("Shader Compilation Error: {0}", info_log);
			}
		}
		else if (action == GL_LINK_STATUS)
		{
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(id, 512, NULL, info_log);
				LOG_CORE_ERROR("Shader Linking Error: ", info_log);
			}
		}

		return !!success;
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

	void Shader::SetMat(cstring name, float* value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, name), 1, GL_FALSE, value);
	}

	void Shader::SetVec(cstring name, float* value)
	{
		glUniform3fv(glGetUniformLocation(m_ProgramId, name), 1, value);
	}

} // namespace polos

#endif // USE_OPENGL
