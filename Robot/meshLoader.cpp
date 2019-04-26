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

MeshLoader::vpn_mesh_t mini::MeshLoader::CreateCylinder(float radius, float height, int r_count)
{
	vpn_verts_t vpn;
	indices_t indices;

	for (size_t i = 0; i < r_count; i++)
	{
		float angle = float(i) / float(r_count - 1) * XM_2PI;
		vpn.push_back({ XMFLOAT3(0.5f * height, radius * cos(angle), radius * sinf(angle)), XMFLOAT3(1.0f, 0.0f, 0.0f) });
	}

	for (size_t i = 0; i < r_count - 2; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}

	for (size_t i = 0; i < r_count; i++)
	{
		float angle = float(i) / float(r_count - 1) * XM_2PI;
		vpn.push_back({ XMFLOAT3(-0.5f * height, radius * cos(angle), radius * sinf(angle)), XMFLOAT3(-1.0f, 0.0f, 0.0f) });
	}

	for (size_t i = 0; i < r_count - 2; i++)
	{
		indices.push_back(r_count);
		indices.push_back(r_count + i + 2);
		indices.push_back(r_count + i + 1);
	}

	size_t idx = vpn.size();

	vpn.push_back({ vpn[0].position, XMFLOAT3(0.0f, vpn[0].position.y, vpn[0].position.z) });
	vpn.push_back({ vpn[r_count].position, 	XMFLOAT3(0.0f, vpn[r_count].position.y, vpn[r_count].position.z) });
	
	for (size_t i = 0; i < r_count - 1; i++, idx += 2)
	{
		vpn.push_back({ vpn[i + 1].position, XMFLOAT3(0.0f, vpn[i + 1].position.y, vpn[i + 1].position.z) });
		vpn.push_back({ vpn[r_count + i + 1].position, 	XMFLOAT3(0.0f, vpn[r_count + i + 1].position.y, vpn[r_count + i + 1].position.z) });

		indices.push_back(idx);
		indices.push_back(idx + 1);
		indices.push_back(idx + 2);

		indices.push_back(idx + 1);
		indices.push_back(idx + 3);
		indices.push_back(idx + 2);
	}

	return vpn_mesh_t(vpn, indices);
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

PumaData MeshLoader::LoadPumaMesh(const std::wstring& fileName)
{
	ifstream input;
	input.exceptions(ios::badbit | ios::failbit | ios::eofbit);
	input.open(fileName);

	int vertices_count, vert_nr, vn, in, en;

	input >> vertices_count;
	vector<DirectX::XMFLOAT3> vertices(vertices_count);

	for (auto i = 0; i < vertices_count; ++i)
		input >> vertices[i].x >> vertices[i].y >> vertices[i].z;

	input >> vn;
	vector<VertexPositionNormal> verts(vn);

	for (auto i = 0; i < vn; i++)
	{
		input >> vert_nr;
		verts[i].position.x = vertices[vert_nr].x;
		verts[i].position.y = vertices[vert_nr].y;
		verts[i].position.z = vertices[vert_nr].z;

		input >> verts[i].normal.x >> verts[i].normal.y >> verts[i].normal.z;
	}

	input >> in;
	in *= 3;
	vector<unsigned short> inds(in);
	for (auto i = 0; i < in; ++i)
		input >> inds[i];



	input >> en;
	vector<Edge> edges(en);
	for (auto i = 0; i < en; ++i)
	{
		Edge e;

		input >> vert_nr;
		e.PositionLeft = vertices[vert_nr];

		input >> vert_nr;
		e.PositionRight = vertices[vert_nr];

		input >> e.TriangleIdxDown;
		input >> e.TriangleIdxTop;

		edges[i] = e;
	}

	return{ move(verts), move(inds), move(edges) };
}
