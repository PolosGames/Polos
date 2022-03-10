#pragma once
#ifndef POLOS_RENDERER_CONTEXT_SHADER_H_
#define POLOS_RENDERER_CONTEXT_SHADER_H_

#include "utils/alias.h"

namespace polos
{
	class Shader
	{
	public:
		Shader() = default;
		Shader(cstring vert_file, cstring frag_file);
		~Shader();

		void Load(cstring vert_file, cstring frag_file);
		void Use() const;

		void SetInt(cstring name, int32 value);
		void SetFloat(cstring name, float value);
		void SetDouble(cstring name, double value);
		void SetUint(cstring name, uint32 value);
		void SetMat(cstring name, float* value);
		void SetVec(cstring name, float* value);
	private:
		static bool is_successful(uint32 id, uint32 action);

	private:
		uint32 m_ProgramId;
		// TODO: Test if getuniformlocation is faster than creating a lookup table
		static HashMap<std::string, uint32> m_UniformLookup;
	};
}

#endif /* POLOS_RENDERER_CONTEXT_SHADER_H_ */