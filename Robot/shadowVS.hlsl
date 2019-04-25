cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0 - matches slot in vsBilboard.hlsl
{
	matrix worldMatrix;
};

struct VSInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
};

struct GSInput
{
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
};


GSInput main(VSInput i)
{
	GSInput o;
	o.worldPos = mul(worldMatrix, float4(i.pos, 1.0f)).xyz;
	o.norm = mul(worldMatrix, float4(i.norm, 0.0f)).xyz;
	o.norm = normalize(o.norm);
	return o;
}