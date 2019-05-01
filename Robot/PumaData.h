#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "vertexDef.h"

namespace mini
{
	struct Edge {
		DirectX::XMFLOAT3 PositionRight;
		DirectX::XMFLOAT3 PositionLeft;
		unsigned short TriangleIdxTop;
		unsigned short TriangleIdxDown;

		void Flip() {
			std::swap(TriangleIdxDown, TriangleIdxTop);
			std::swap(PositionLeft, PositionRight);
		}
	};

	struct PumaData {
		std::vector<VertexPositionNormal> verts;
		std::vector<unsigned short> indices;
		std::vector<Edge> edges;
	};
}
