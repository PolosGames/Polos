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
        struct out_variable
        {
            int32 location;
            int32 count;
        };
    public:
        Shader() : m_ProgramId{uint32(-1)} {}
        explicit Shader(uint32 program_id);
        void CreateUniformLookup();
        void Use() const;
        
        void SetInt(StringId name, int32 value);
        void SetUint(StringId name, uint32 value);
        void SetFloat(StringId name, float value);
        void SetDouble(StringId name, double value);
        void SetMat(StringId name, glm::mat4 const& value);
        void SetVec(StringId name, glm::vec2 const& value);
        void SetVec(StringId name, glm::vec3 const& value);
        void SetVec(StringId name, glm::vec4 const& value);
    private:
        uint32 m_ProgramId;
        HashMap<StringId, out_variable> m_LookupTable;
    };
}
#endif //POLOS_SHADER_H_
