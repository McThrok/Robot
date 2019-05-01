cbuffer cbProj : register(b0) //Geometry Shader constant buffer slot 0
{
	matrix projMatrix;
};

struct GSInput
{
	float4 pos : POSITION;
	float age : TEXCOORD0;
	float angle : TEXCOORD1;
	float size : TEXCOORD2;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex1: TEXCOORD0;
	float2 tex2: TEXCOORD1;
};

static const float TimeToLive = 4.0f;

[maxvertexcount(4)]
void main(point GSInput inArray[1], inout TriangleStream<PSInput> ostream)
{
	GSInput i = inArray[0];
	float sina, cosa;
	sincos(i.angle, sina, cosa);
	float dx = (cosa - sina) * 0.5 * i.size;
	float dy = (cosa + sina) * 0.5 * i.size;
	PSInput o = (PSInput)0;

	// TODO : 2.28 Initialize 4 vertices to make a bilboard and append them to the ostream
	o.tex2.x = i.age / TimeToLive;
	o.tex2.y = 0.5f;

	o.pos = i.pos;
	o.pos.x -= dx;
	o.pos.y -= dy;
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 0;
	o.tex1.y = 1;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos.x -= dy;
	o.pos.y += dx;
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 0;
	o.tex1.y = 0;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos.x += dy;
	o.pos.y -= dx;
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 1;
	o.tex1.y = 1;
	ostream.Append(o);

	o.pos = i.pos;
	o.pos.x += dx;
	o.pos.y += dy;
	o.pos = mul(projMatrix, o.pos);
	o.tex1.x = 1;
	o.tex1.y = 0;
	ostream.Append(o);

	ostream.RestartStrip();
}