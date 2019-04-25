#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "vertexDef.h"

namespace mini
{
	struct Edge {
		DirectX::XMFLOAT3 PositionA;
		DirectX::XMFLOAT3 PositionB;
		unsigned short TriangleIdxA;
		unsigned short TriangleIdxB;
	};

	struct PumaData {
		std::vector<VertexPositionNormal> verts;
		std::vector<unsigned short> indices;
		std::vector<Edge> edges;
	};
}