cbuffer cbWorld : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1)
{
	matrix viewMatrix;
	matrix invViewMatrix;
};

cbuffer cbProj : register(b2)
{
	matrix projMatrix;
};

cbuffer cbLights : register(b3)
{
	float4 lightPos;
};


struct GSInput
{
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
};


struct PSInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL0;
	float3 worldPos : POSITION0;
};

float4 GetScreenPos(float3 pos) {
	return mul(projMatrix, mul(viewMatrix, float4(pos, 1.0f)));
}

[maxvertexcount(3)]
void main(triangle GSInput inArray[3], inout TriangleStream<PSInput> ostream)
{

	PSInput o = (PSInput)0;

	GSInput i = inArray[0];
	o.worldPos = i.worldPos;
	o.pos = GetScreenPos(o.worldPos);
	o.norm = i.norm;
	ostream.Append(o);

	i = inArray[1];
	o.worldPos = i.worldPos;
	o.pos = GetScreenPos(o.worldPos);
	o.norm = i.norm;
	ostream.Append(o);


	i = inArray[2];
	o.worldPos = i.worldPos;
	o.pos = GetScreenPos(o.worldPos);
	o.norm = i.norm;
	ostream.Append(o);

	ostream.RestartStrip();
}