cbuffer constants : register(b0) {
    float2 offset;
    float4 horizontalScale;
}

float4 main(float2 texcoord : TEXCOORD, float4 color : COLOR) : SV_TARGET{
    return color;
}