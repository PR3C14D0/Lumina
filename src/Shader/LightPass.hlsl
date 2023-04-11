struct VertexOutput
{
    float4 position : SV_Position;
};

Texture2DMS<float4> albedo : register(t0);
Texture2DMS<float4> normal : register(t1);
Texture2DMS<float4> position : register(t2);

VertexOutput VertexMain(float4 position : POSITION)
{
    VertexOutput output;
    output.position = position;
    return output;
}

float4 PixelMain(VertexOutput input, uint sampleIndex : SV_SampleIndex) : SV_Target {
    float4 albedoColor = albedo.Load(input.position.xy, sampleIndex);
    float4 nml = normalize(normal.Load(input.position.xy, sampleIndex) * 2 - 1); // Unpack normals.
    float4 vertexPos = position.Load(input.position.xy, sampleIndex);
    
    float4 ambientColor = float4(0.1f, 0.1f, 0.1f, 1.f);
    float3 lightPos = float3(1.f, 1.f, -2.f);
    float4 lightColor = float4(1.f, 1.f, 1.f, 1.f);
    
    float3 lightDir = normalize(lightPos - vertexPos.xyz);
    float NdotL = dot(lightDir, nml.xyz);
    float diffuseColor = saturate(NdotL);
    
    albedoColor = saturate(saturate(diffuseColor + ambientColor) * lightColor) * albedoColor;
    
    return albedoColor;
}