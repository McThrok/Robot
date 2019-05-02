cbuffer cbView : register(b0) //Vertex Shader constant buffer slot 1
{
	matrix viewMatrix;
};

cbuffer plateView : register(b1) //Vertex Shader constant buffer slot 2
{
	matrix plateMatrix;
};

struct VSInput
{
	float3 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
};

GSInput main(VSInput i)
{
	GSInput o = (GSInput)0;
	o.pos = float4(i.pos, 1.0f);
	o.pos = mul(plateMatrix, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos.y -= i.age * i.age;
	o.age = i.age;
	o.angle = i.angle;
	o.size = i.size;
	return o;
}