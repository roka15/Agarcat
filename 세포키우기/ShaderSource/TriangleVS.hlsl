

struct VSIn
{
	float3 pos:POSITION;
	float4 color:COLOR;
};
struct VSOut
{
	float4 pos:SV_POSITION;
	float4 color:COLOR;
};
cbuffer Transform : register(b0)
{
	float4 Pos;
}
VSOut main( VSIn In )
{
	VSOut outData = (VSOut)0.0f;
	outData.pos = float4(In.pos, 1.0f);
	outData.pos.x += Pos.x;
	outData.pos.y += Pos.y;

	outData.color = In.color;
	return outData;
}