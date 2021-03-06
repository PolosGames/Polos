#ifndef POLOS_SHADER_H_
#define POLOS_SHADER_H_

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

        void SetUniform(StringId const name, int32 const value)         const;
        void SetUniform(StringId const name, glm::i32vec2 const& value) const;
        void SetUniform(StringId const name, glm::i32vec3 const& value) const;
        void SetUniform(StringId const name, glm::i32vec4 const& value) const;
        void SetUniform(StringId const name, std::span<int32> value)    const;

        void SetUniform(StringId const name, uint32 const value)        const;
        void SetUniform(StringId const name, glm::u32vec2 const& value) const;
        void SetUniform(StringId const name, glm::u32vec3 const& value) const;
        void SetUniform(StringId const name, glm::u32vec4 const& value) const;
        void SetUniform(StringId const name, std::span<uint32> value)   const;

        void SetUniform(StringId const name, float const value)      const;
        void SetUniform(StringId const name, glm::vec2 const& value) const;
        void SetUniform(StringId const name, glm::vec3 const& value) const;
        void SetUniform(StringId const name, glm::vec4 const& value) const;
        void SetUniform(StringId const name, std::span<float> value) const;

        void SetUniform(StringId const name, double const value)      const;
        void SetUniform(StringId const name, glm::dvec2 const& value) const;
        void SetUniform(StringId const name, glm::dvec3 const& value) const;
        void SetUniform(StringId const name, glm::dvec4 const& value) const;
        void SetUniform(StringId const name, std::span<double> value) const;

        void SetUniform(StringId const name, glm::mat2 const& value) const;
        void SetUniform(StringId const name, glm::mat3 const& value) const;
        void SetUniform(StringId const name, glm::mat4 const& value) const;
        
    public:    
        uint32 m_ProgramId;
    private:
        HashMap<StringId, out_variable> m_LookupTable;
    };
}
#endif //POLOS_SHADER_H_
