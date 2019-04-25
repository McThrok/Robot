#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace mini
{
	struct Edge {
		DirectX::XMFLOAT3 PositionA;
		DirectX::XMFLOAT3 PositionB;
		unsigned short TriangleIdxA;
		unsigned short TriangleIdxB;
	};
}