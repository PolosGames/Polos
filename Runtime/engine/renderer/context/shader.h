#ifndef POLOS_SHADER_H_
#define POLOS_SHADER_H_

#include <glm/glm.hpp>

namespace polos
{
    enum ShaderType
    {
        kVertex,
        kFragment,
        kGeometry,
        kCompute,
        kShaderTypeMax,
    };
    
    class Shader
    {
    public:
        Shader() {}
        explicit Shader(uint32 program_id);
        void Use() const;
        
        void SetInt(cstring name, int32 value);
        void SetUint(cstring name, uint32 value);
        void SetFloat(cstring name, float value);
        void SetDouble(cstring name, double value);
        void SetMat(cstring name, glm::mat4 const& value);
        void SetVec(cstring name, glm::vec2 const& value);
        void SetVec(cstring name, glm::vec3 const& value);
        void SetVec(cstring name, glm::vec4 const& value);
    private:
        uint32 m_ProgramId;
    };
}
#endif //POLOS_SHADER_H_
