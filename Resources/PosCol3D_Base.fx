//////////////////////////////////
//   Input/Output structs
//////////////////////////////////
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

//////////////////////////////////
//   Keeping track of all settings
//////////////////////////////////
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samAnisotropic
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////////////////////////
//   Vertex Shader
//////////////////////////////////
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    
    return output;
}

//////////////////////////////////
//   Pixel shader
//////////////////////////////////
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    input.Color = gDiffuseMap.Sample(samPoint, input.TexCoord);
    return input.Color;
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    input.Color = gDiffuseMap.Sample(samLinear, input.TexCoord);
    return input.Color;
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    input.Color = gDiffuseMap.Sample(samAnisotropic, input.TexCoord);
    return input.Color;
}

//////////////////////////////////
//   Technique
//////////////////////////////////
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}