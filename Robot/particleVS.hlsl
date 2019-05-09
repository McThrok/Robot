cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0 - matches slot in vsBilboard.hlsl
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbPlate : register(b2) //Vertex Shader constant buffer slot 2
{
	matrix plateMatrix;
	matrix invPlateMatrix;
};

struct VSInput
{
	float3 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
	float3 velocity : NORMAL0;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
	float4x4 worldMatrix : MYMATRIX;
};

static const float4 gravityVec = float4(0, -1, 0, 0);

GSInput main(VSInput i)
{
	GSInput o = (GSInput)0;
	o.pos = float4(i.pos, 1.0f);
	o.pos += float4(i.velocity * i.age, 0); //initial speed
	o.pos -= float4(i.velocity * i.age * i.age / 2, 0); //initial speed vanishing
	o.pos += mul(invPlateMatrix, gravityVec * i.age * i.age); //gravity

	float3 dVel = i.velocity - i.velocity * i.age + mul(invPlateMatrix, gravityVec * i.age);

	float4 viewPos = mul(invViewMatrix, float4(0, 0, 0, 1));
	float4 viewVec = mul(invPlateMatrix, viewPos) - o.pos;

	float4 z = normalize(float4(-dVel, 0));
	float4 y = float4(normalize(cross(z, -viewVec)), 0);
	float4 x = float4(normalize(cross(z, y)), 0);
	float4 w = float4(0, 0, 0, 1);

	float4x4 mat = transpose(float4x4(x, y, z, w));

	o.worldMatrix = mul(mat, worldMatrix);
	o.age = i.age;
	o.angle = i.angle;
	o.size = i.size;
	return o;
}
