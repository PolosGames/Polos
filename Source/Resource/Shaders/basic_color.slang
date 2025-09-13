// Output of the vertex shader, and input to the fragment shader.
struct CoarseVertex
{
    float3 color;
};

// Vertex  Shader
struct VertexStageOutput
{
    CoarseVertex coarseVertex : CoarseVertex;
    float4       sv_position : SV_Position;
};

// clang-format off
static const float2 k_positions[3] = {
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
};

static const float3 k_colors[3] = {
    float3(1.0, 0.0, 0.0),// Red
    float3(0.0, 1.0, 0.0),// Green
    float3(0.0, 0.0, 1.0) // Blue
};
// clang-format on

[shader("vertex")]
VertexStageOutput vertexMain(uint vertexId: SV_VertexID)
{
    VertexStageOutput output;

    // Look up position and color using the vertex index.
    output.sv_position        = float4(k_positions[vertexId], 0.0, 1.0);
    output.coarseVertex.color = k_colors[vertexId];

    return output;
}

// Fragment Shader
[shader("fragment")]
float4 fragmentMain(CoarseVertex coarseVertex: CoarseVertex)
    : SV_Target
{
    // Add an alpha component of 1.0 to the interpolated color.
    return float4(coarseVertex.color, 1.0);
}
