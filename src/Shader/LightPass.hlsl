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
    return albedo.Load(input.position.xy, sampleIndex);
}