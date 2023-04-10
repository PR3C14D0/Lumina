struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 vertexPos : POSITION0;
};

cbuffer WVP : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

VertexOutput VertexMain(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VertexOutput output;
    output.position = mul(position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.normal = normal;
    output.uv = uv;
    output.vertexPos = position;
    return output;
}

struct PixelOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 position : SV_Target2;
};

PixelOutput PixelMain(VertexOutput input)
{
    PixelOutput output;
    output.albedo = float4(1.f, 0.f, 0.f, 1.f);
    output.normal = input.normal * 0.5f + 0.5f;
    output.position = input.vertexPos;
    return output;
}