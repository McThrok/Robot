cbuffer cbView : register(b0) //Vertex Shader constant buffer slot 1
{
	matrix viewMatrix;
};

cbuffer cbProj : register(b1) //Geometry Shader constant buffer slot 0
{
	matrix projMatrix;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
	float4x4 a : MYMATRIX;
	float4x4 b : MYPLATEMATRIX;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex1: TEXCOORD0;
	float2 tex2: TEXCOORD1;
};

static const float TimeToLive = 1.0f;

[maxvertexcount(4)]
void main(point GSInput inArray[1], inout TriangleStream<PSInput> ostream)
{
	GSInput i = inArray[0];
	i.angle = 0;
	//	i.pos = mul(viewMatrix, i.pos);

	float sina, cosa;
	sincos(i.angle, sina, cosa);
	float dx = (cosa - sina) * 0.5 * i.size;
	float dy = (cosa + sina) * 0.5 * i.size;
	PSInput o = (PSInput)0;

	float4 myVec = float4(i.pos.xyz, 0);

	// Initialize 4 vertices to make a bilboard and append them to the ostream
	o.tex2.x = i.age / TimeToLive;
	o.tex2.y = 0.5f;


	o.pos = i.pos;
	o.pos += mul(i.a, float4(-dx, -dy, 0, 0));
	//o.pos.x -= dx;
	//o.pos.y -= dy;
	o.pos = mul(i.b, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 0;
	o.tex1.y = 1;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos += mul(i.a, float4(-dy, dx, 0, 0));
	//o.pos.x -= dy;
	//o.pos.y += dx;
	o.pos = mul(i.b, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 0;
	o.tex1.y = 0;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos += mul(i.a, float4(dy, -dx, 0, 0));
	//o.pos.x += dy;
	//o.pos.y -= dx;
	o.pos = mul(i.b, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 1;
	o.tex1.y = 1;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos += mul(i.a, float4(dx, dy, 0, 0));
	//o.pos.x += dx;
	//o.pos.y += dy;
	o.pos = mul(i.b, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 1;
	o.tex1.y = 0;
	ostream.Append(o);

	ostream.RestartStrip();
}