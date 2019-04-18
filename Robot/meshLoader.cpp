#include "meshLoader.h"
#include <fstream>

using namespace std;
using namespace DirectX;
using namespace mini;


MeshLoader::vpn_mesh_t MeshLoader::CreatePentagon(float radius)
{
	vpn_verts_t vertices;
	vertices.reserve(5);
	float a = 0, da = XM_2PI / 5.0f;
	for (int i = 0; i < 5; ++i, a -= da)
	{
		float sina, cosa;
		XMScalarSinCos(&sina, &cosa, a);
		vertices.push_back({ XMFLOAT3{ cosa*radius, sina*radius, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, -1.0f } });
	}
	return{ move(vertices), { 0, 1, 2, 0, 2, 3, 0, 3, 4 } };
}

MeshLoader::vpn_verts_t MeshLoader::BoxVertices(float width, float height, float depth)
{
	return{
		//Front face
		{ XMFLOAT3(-0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		//Back face
		{ XMFLOAT3(+0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		//Left face
		{ XMFLOAT3(-0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		//Right face
		{ XMFLOAT3(+0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		//Bottom face
		{ XMFLOAT3(-0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f*width, -0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		//Top face
		{ XMFLOAT3(-0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, -0.5f*depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, +0.5f*depth), XMFLOAT3(0.0f, 1.0f, 0.0f) },
	};
}

MeshLoader::indices_t MeshLoader::BoxIndices()
{
	return {
		0,2,1, 0,3,2,
		4,6,5, 4,7,6,
		8,10,9, 8,11,10,
		12,14,13, 12,15,14,
		16,18,17, 16,19,18,
		20,22,21, 20,23,22
	};
}

MeshLoader::vpn_mesh_t MeshLoader::CreateDoubleSidedRectangle(float width, float height)
{
	return{
	{
		{ XMFLOAT3(-0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(+0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(+0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) }
	},
	{ 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7 } };
}

MeshLoader::vp_mesh_t MeshLoader::CreateRectangleBilboard(float width, float height)
{
	return{
		{
			{-0.5f*width, -0.5f*height, 0.0f},
			{-0.5f*width, +0.5f*height, 0.0f},
			{+0.5f*width, +0.5f*height, 0.0f},
			{+0.5f*width, -0.5f*height, 0.0f}
		},
		{ 0, 1, 2, 0, 2, 3 }
	};
}

MeshLoader::vpn_mesh_t MeshLoader::CreateRectangle(float width, float height)
{
	return{
		{
			{ XMFLOAT3(-0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(-0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(+0.5f*width, +0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(+0.5f*width, -0.5f*height, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) }
		},
		{ 0, 1, 2, 0, 2, 3 }
	};
}

MeshLoader::vpn_mesh_t MeshLoader::LoadMesh(const std::wstring& fileName)
{
	//File format for VN vertices and IN indices (IN divisible by 3, i.e. IN/3 triangles):
	//VN IN
	//pos.x pos.y pos.z norm.x norm.y norm.z tex.x tex.y [VN times, i.e. for each vertex]
	//t.i1 t.i2 t.i3 [IN/3 times, i.e. for each triangle]

	ifstream input;
	// In general we shouldn't throw exceptions on end-of-file,
	// however, in case of this file format if we reach the end
	// of a file before we read all values, the file is
	// ill-formated and we would need to throw an exception anyway
	input.exceptions(ios::badbit | ios::failbit | ios::eofbit);
	input.open(fileName);

	int vn, in;
	input >> vn >> in;

	vector<VertexPositionNormal> verts(vn);
	XMFLOAT2 ignoreTextureCoords;
	for (auto i = 0; i < vn; ++i)
		input >> verts[i].position.x >> verts[i].position.y >> verts[i].position.z
			>> verts[i].normal.x >> verts[i].normal.y >> verts[i].normal.z
			>> ignoreTextureCoords.x >> ignoreTextureCoords.y;

	vector<unsigned short> inds(in);
	for (auto i = 0; i < in; ++i)
		input >> inds[i];

	return{ move(verts), move(inds) };
}