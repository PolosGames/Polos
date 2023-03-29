#pragma once

namespace polos
{
    enum ShaderType
    {
        k_Vertex,
        k_Fragment,
        k_Geometry,
        k_Compute,
        k_ShaderTypeMax,
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
        explicit Shader(uint32 m_ProgramId);

        void CreateUniformLookup();
        void Use() const;
        void Release() const;

        void SetUniform(StringId const p_Name, int32 const p_Value)         const;
        void SetUniform(StringId const p_Name, glm::i32vec2 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::i32vec3 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::i32vec4 const& p_Value) const;
        void SetUniform(StringId const p_Name, std::span<int32> p_Value)    const;

        void SetUniform(StringId const p_Name, uint32 const p_Value)        const;
        void SetUniform(StringId const p_Name, glm::u32vec2 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::u32vec3 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::u32vec4 const& p_Value) const;
        void SetUniform(StringId const p_Name, std::span<uint32> p_Value)   const;

        void SetUniform(StringId const p_Name, float const p_Value)      const;
        void SetUniform(StringId const p_Name, glm::vec2 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::vec3 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::vec4 const& p_Value) const;
        void SetUniform(StringId const p_Name, std::span<float> p_Value) const;

        void SetUniform(StringId const p_Name, double const p_Value)      const;
        void SetUniform(StringId const p_Name, glm::dvec2 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::dvec3 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::dvec4 const& p_Value) const;
        void SetUniform(StringId const p_Name, std::span<double> p_Value) const;

        void SetUniform(StringId const p_Name, glm::mat2 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::mat3 const& p_Value) const;
        void SetUniform(StringId const p_Name, glm::mat4 const& p_Value) const;
        
    public:    
        uint32 m_ProgramId;
    private:
        HashMap<StringId, out_variable> m_LookupTable;
    };
} // namespace polos
