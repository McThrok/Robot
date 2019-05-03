#include <array>
#include "Scene.h"
#include "meshLoader.h"
#include <DirectXMath.h>
using namespace mini;
using namespace utils;
using namespace gk2;
using namespace DirectX;
using namespace std;

const unsigned int Scene::BS_MASK = 0xffffffff;
const float Scene::VOLUME_OFFSET = 4.0f;
const XMFLOAT4 Scene::LIGHT_POS = { -1.0f, 1.0f, -1.0f, 1.0f };
const XMVECTOR Scene::MIRROR_AXIS = { sqrt(3), 1, 0 };

const XMFLOAT4 Scene::WHITE_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
const XMFLOAT4 Scene::BLACK_COLOR = { 0.0f, 0.0f, 0.0f, 1.0f };
const XMFLOAT4 Scene::PUMA_COLOR = { 125.0f / 255.0f, 167.0f / 255.0f, 216.0f / 255.0f, 100.0f / 255.0f };
const XMFLOAT4 Scene::WALLS_COLORS[] = {
	XMFLOAT4(255.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(255.0f / 255.0f, 241.0f / 255.0f, 224.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(255.0f / 255.0f, 246.0f / 255.0f, 214.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(229.0f / 255.0f, 255.0f / 255.0f, 235.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(251.0f / 255.0f, 229.0f / 255.0f, 255.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(229.0f / 255.0f, 245.0f / 255.0f, 255.0f / 255.0f, 100.0f / 255.0f),
};

Scene::Scene(HINSTANCE appInstance) : Gk2ExampleBase(appInstance, 1280, 720, L"Robot"),
//Constant Buffers
m_cbWorldMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbProjMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbMirrorTexMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbViewMtx(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
m_cbPlateMtx(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
m_cbLightColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
m_cbLightPos(m_device.CreateConstantBuffer<XMFLOAT4>()),
//Textures
m_mirrorTexture(m_device.CreateShaderResourceView(L"resources/textures/mirror_texture.png"))
{
	// Projection matrix
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	m_cbProjMtx.Update(m_device.context(), m_projMtx);

	CreateRenderStates();

	// Meshes
	vector<VertexPositionNormal> vertices;
	vector<unsigned short> indices;

	// Light
	tie(vertices, indices) = MeshLoader::CreateBox(0.05f);
	m_light = m_device.CreateMesh(indices, vertices);

	XMStoreFloat4x4(&m_lightMtx, XMMatrixTranslation(LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z));

	// Walls
	tie(vertices, indices) = MeshLoader::CreateSquare(4.0f);
	m_wall = m_device.CreateMesh(indices, vertices);

	XMMATRIX temp = XMMatrixTranslation(0.0f, 1.0f, 2.0f);
	float a = 0.f;
	for (auto i = 0U; i < 4U; ++i, a += XM_PIDIV2)
		XMStoreFloat4x4(&m_wallsMtx[i], temp * XMMatrixRotationY(a)* XMMatrixTranslation(-0.5f, 0.0f, 0.0f));
	XMStoreFloat4x4(&m_wallsMtx[4], XMMatrixTranslation(-0.5f, 0.0f, 1.0f) * XMMatrixRotationX(XM_PIDIV2));
	XMStoreFloat4x4(&m_wallsMtx[5], XMMatrixTranslation(-0.5f, 0.0f, 3.0f) * XMMatrixRotationX(-XM_PIDIV2));

	// Plate
	tie(vertices, indices) = MeshLoader::CreateSquare(1.5f);
	for (size_t i = 0; i < 2; i++)
		m_plate[i] = m_device.CreateMesh(indices, vertices);

	XMStoreFloat4x4(&m_plateMtx[0], XMMatrixRotationX(XM_PI / 6)
		* XMMatrixRotationY(-XM_PIDIV2)
		* XMMatrixTranslation(-1.5f, 0.2f, 0.0f));

	XMStoreFloat4x4(&m_plateMtx[1], XMMatrixRotationX(-XM_PI / 6)
		* XMMatrixRotationY(XM_PIDIV2)
		* XMMatrixTranslation(-1.5f, 0.2f, 0.0f));

	XMMATRIX m_scale = XMMatrixScaling(1.0f, 1.0f, -1.0f);
	XMMATRIX m = XMLoadFloat4x4(&m_plateMtx[0]);
	XMMATRIX m_inverse = XMMatrixInverse(nullptr, m);
	XMStoreFloat4x4(&m_mirrorMtx, m_inverse * m_scale * m);

	// Cylinder
	tie(vertices, indices) = MeshLoader::CreateCylinder(0.4f, 2.0f, 20);
	m_cylinder = m_device.CreateMesh(indices, vertices);
	XMStoreFloat4x4(&m_cylinderMtx, XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(1.0f, 0.0f, 1.5f));

	// Puma
	for (size_t i = 0; i < 6; i++)
	{
		wstring path = L"resources/puma/mesh" + to_wstring(i + 1) + L".txt";
		PumaData data = MeshLoader::LoadPumaMesh(path);
		m_pumaData[i] = data;
		m_puma[i] = m_device.CreateMesh(data.indices, data.verts);
	}

	for (size_t i = 0; i < 6; i++)
		XMStoreFloat4x4(&m_pumaMtx[i], XMMatrixIdentity());

	// Phong
	auto vsCode = m_device.LoadByteCode(L"phongVS.cso");
	auto psCode = m_device.LoadByteCode(L"phongPS.cso");

	m_phongEffect = PhongEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
		m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbLightPos, m_cbSurfaceColor, m_cbLightColor);
	m_inputlayout = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);

	// Mirror texture
	vsCode = m_device.LoadByteCode(L"texturedVS.cso");
	psCode = m_device.LoadByteCode(L"texturedPS.cso");

	SamplerDescription sd;
	sd.AddressU = sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	m_samplerWrap = m_device.CreateSamplerState(sd);

	XMFLOAT4X4 tempMtx;
	XMStoreFloat4x4(&tempMtx, XMMatrixIdentity());
	m_cbMirrorTexMtx.Update(m_device.context(), tempMtx);

	m_mirrorTexturedEffect = TexturedEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
		m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbMirrorTexMtx, m_samplerWrap, m_mirrorTexture);

	// Particles

	XMMATRIX mtx = XMLoadFloat4x4(&m_plateMtx[0]);
	auto invMtx = XMMatrixInverse(nullptr, mtx);
	XMFLOAT4X4 plate[2] = { m_plateMtx[0] };
	XMStoreFloat4x4(&plate[1], invMtx);
	m_cbPlateMtx.Update(m_device.context(), plate);

	XMStoreFloat4x4(&m_particleMtx, XMMatrixRotationY(XM_PIDIV2));

	m_particles = ParticleSystem(m_device, m_cbWorldMtx, m_cbViewMtx, m_cbPlateMtx, m_cbProjMtx, m_samplerWrap, XMFLOAT3(1.3f, -0.6f, 1.0f));

	//Constant buffers content
	m_cbLightPos.Update(m_device.context(), LIGHT_POS);

	m_device.context()->IASetInputLayout(m_inputlayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Scene::CreateRenderStates()
{
	DepthStencilDescription dssDesc;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_dssNoWrite = m_device.CreateDepthStencilState(dssDesc);
	m_dssWrite = m_device.CreateDepthStencilState(dssDesc.StencilWriteDescription());
	m_dssTest = m_device.CreateDepthStencilState(dssDesc.StencilTestDescription());
	dssDesc = dssDesc.StencilTestDescription();
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_dssTestNoWrite = m_device.CreateDepthStencilState(dssDesc);
	m_rsCCW = m_device.CreateRasterizerState(RasterizerDescription(true));

	BlendDescription bsDesc;
	m_bsAlpha = m_device.CreateBlendState(bsDesc.AlphaBlendDescription());

	//m_bdNoDraw
	BlendDescription desc = desc.AdditiveBlendDescription();
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	m_bsNoDraw = m_device.CreateBlendState(desc);

	//m_rsInitShadow
	RasterizerDescription rs = RasterizerDescription(true);
	rs.CullMode = D3D11_CULL_NONE;
	m_rsInitShadow = m_device.CreateRasterizerState(rs);

	//m_dssInitShadow
	dssDesc = DepthStencilDescription();
	dssDesc.StencilEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	m_dssInitShadow = m_device.CreateDepthStencilState(dssDesc);

	//m_dssRenderNoShadow 
	dssDesc = DepthStencilDescription();
	dssDesc.StencilEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	m_dssRenderNoShadow = m_device.CreateDepthStencilState(dssDesc);

	//m_dssRenderShadow
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	m_dssRenderShadow = m_device.CreateDepthStencilState(dssDesc);
}

void Scene::UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx)
{
	XMMATRIX mtx = XMLoadFloat4x4(&cameraMtx);
	XMVECTOR det;
	auto invvmtx = XMMatrixInverse(&det, mtx);
	XMFLOAT4X4 view[2] = { cameraMtx };
	XMStoreFloat4x4(view + 1, invvmtx);
	m_cbViewMtx.Update(m_device.context(), view);
}

void Scene::Update(const Clock& c)
{
	double dt = c.getFrameTime();
	angle += dt;

	HandleCameraInput(dt);

	//XMVECTOR pos = XMVector3Transform({ 0, 0, 0, 1 }, XMMatrixTranslation(0.0f, 0.0f, 0.5f)
	//	* XMMatrixRotationAxis({ 1, 0, 0 }, angle) * XMMatrixTranslation(-1.5f, 0.2f, 0.0f));
	//XMVECTOR pos = XMVector3Transform({ 0, 0, 0, 1 }, XMMatrixTranslation(0.0f, 0.0f, 0.5f)
	//	* XMMatrixRotationAxis(MIRROR_AXIS, angle) * XMMatrixTranslation(-1.5f, 0.2f, 0.0f));

	XMVECTOR pos = XMVector3Transform({ 0, 0, 0, 1 }, XMMatrixTranslation(0.0f, 0.5f, 0.0f)
		*XMMatrixRotationZ(-angle) * XMLoadFloat4x4(&m_plateMtx[0]));
	UpdateRobotMtx(dt, pos);

	XMFLOAT3 emitterPos;
	XMStoreFloat3(&emitterPos, pos);
	UpdateParticles(dt, emitterPos);

	for (size_t i = 0; i < 6; i++)
		UpdateShadowVolume(i);
}

void Scene::UpdateRobotMtx(float dt, XMVECTOR pos)
{
	float a1, a2, a3, a4, a5;
	InverseKinematics(pos, MIRROR_AXIS, a1, a2, a3, a4, a5);

	XMStoreFloat4x4(&m_pumaMtx[1], XMMatrixRotationY(a1));

	XMMATRIX m = XMLoadFloat4x4(&m_pumaMtx[1]);
	XMStoreFloat4x4(&m_pumaMtx[2], XMMatrixTranslation(0.0f, -0.27f, 0.0f)
		* XMMatrixRotationZ(a2) * XMMatrixTranslation(0.0f, 0.27f, 0.0f) * m);

	m = XMLoadFloat4x4(&m_pumaMtx[2]);
	XMStoreFloat4x4(&m_pumaMtx[3], XMMatrixTranslation(0.91f, -0.27f, 0.0f)
		* XMMatrixRotationZ(a3) * XMMatrixTranslation(-0.91f, 0.27f, -0.0f) * m);

	m = XMLoadFloat4x4(&m_pumaMtx[3]);
	XMStoreFloat4x4(&m_pumaMtx[4], XMMatrixTranslation(2.05f, -0.27f, 0.26f)
		* XMMatrixRotationX(a4) * XMMatrixTranslation(-2.05f, 0.27f, -0.26f) * m);

	m = XMLoadFloat4x4(&m_pumaMtx[4]);
	XMStoreFloat4x4(&m_pumaMtx[5], XMMatrixTranslation(1.72f, -0.27f, 0.26f)
		* XMMatrixRotationZ(a5) * XMMatrixTranslation(-1.72f, 0.27f, -0.26f) * m);
}

void Scene::UpdateParticles(float dt, XMFLOAT3 emitterPos)
{
	m_particles.Update(m_device.context(), static_cast<float>(dt), m_camera.GetPosition(), emitterPos, angle);
}

void Scene::InverseKinematics(XMVECTOR pos, XMVECTOR normal,
	float & a1, float & a2, float & a3, float & a4, float & a5)
{
	float l1 = 0.91f, l2 = 0.81f, l3 = 0.33f, dy = 0.27f, dz = 0.26f;
	normal = XMVector3Normalize(normal);
	XMFLOAT3 pos_f;
	XMStoreFloat3(&pos_f, pos + normal * l3);

	float e = sqrtf(pos_f.z * pos_f.z + pos_f.x * pos_f.x - dz * dz);
	a1 = atan2(pos_f.z, -pos_f.x) + atan2(dz, e);

	XMFLOAT3 pos2(e, pos_f.y - dy, 0.0f);
	a3 = -acosf(min(1.0f, (pos2.x * pos2.x + pos2.y * pos2.y - l1 * l1 - l2 * l2)
		/ (2.0f * l1 * l2)));

	float k = l1 + l2 * cosf(a3), l = l2 * sinf(a3);
	a2 = -atan2(pos2.y, sqrtf(pos2.x * pos2.x + pos2.z * pos2.z)) - atan2(l, k);

	XMVectorSetW(normal, 0.0f);
	XMVECTOR normal1 = XMVector4Transform(normal, XMMatrixRotationY(-a1));
	XMVectorSetW(normal1, 0.0f);
	normal1 = XMVector4Transform(normal1, XMMatrixRotationZ(-(a2 + a3)));

	a4 = atan2(XMVectorGetZ(normal1), XMVectorGetY(normal1));
	a5 = acosf(XMVectorGetX(normal1));
}

void Scene::DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx)
{
	m_cbWorldMtx.Update(m_device.context(), worldMtx);
	m.Render(m_device.context());
}

void Scene::Render()
{
	Gk2ExampleBase::Render();

	XMMATRIX m_view = m_camera.getViewMatrix();

	XMFLOAT4X4 old_view;
	XMStoreFloat4x4(&old_view, m_view);
	UpdateCameraCB(old_view);

	m_phongEffect.Begin(m_device.context());
	FillStencilShadows();
	RenderScene();
	RenderMirror(m_view);

	DrawParticles();
	m_phongEffect.Begin(m_device.context());
}

void Scene::FillStencilShadows()
{
	m_device.context()->OMSetBlendState(m_bsNoDraw.get(), nullptr, BS_MASK);

	//fill z-buffer
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	DrawWalls();
	DrawCylinder();
	DrawPuma();
	DrawLight();

	//fill shadow stencil
	m_device.context()->RSSetState(m_rsInitShadow.get());
	m_device.context()->OMSetDepthStencilState(m_dssInitShadow.get(), 0);
	DrawShadowVolumes();
	m_device.context()->RSSetState(nullptr);

	m_device.context()->ClearDepthStencilView(getDefaultRenderTarget().getDepthStencilBuffer(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);
}

void Scene::RenderScene()
{
	m_cbSurfaceColor.Update(m_device.context(), WHITE_COLOR);

	//draw non-shady surfaces
	m_cbLightColor.Update(m_device.context(), WHITE_COLOR);
	m_device.context()->OMSetDepthStencilState(m_dssRenderNoShadow.get(), 0);
	DrawPuma();
	DrawWalls();
	DrawCylinder();
	DrawPlateBack();

	//draw shady surfaces
	m_cbLightColor.Update(m_device.context(), BLACK_COLOR);
	m_device.context()->OMSetDepthStencilState(m_dssRenderShadow.get(), 0);
	DrawPuma();
	DrawWalls();
	DrawCylinder();
	DrawPlateBack();

	//light marker
	m_cbLightColor.Update(m_device.context(), WHITE_COLOR);
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	DrawLight();
}

void Scene::RenderMirror(XMMATRIX m_view)
{
	//draw mirror
	m_cbLightColor.Update(m_device.context(), WHITE_COLOR);
	DrawMirroredWorld(m_view);

	//draw mirror texture
	m_device.context()->OMSetDepthStencilState(m_dssTest.get(), 1);
	m_device.context()->OMSetBlendState(m_bsAlpha.get(), nullptr, BS_MASK);
	m_mirrorTexturedEffect.SetTexture(m_mirrorTexture);
	m_mirrorTexturedEffect.Begin(m_device.context());
	DrawMesh(m_plate[0], m_plateMtx[0]);
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);

	m_device.context()->OMSetDepthStencilState(nullptr, 0);
}

void Scene::DrawMirroredWorld(XMMATRIX m_view)
{
	m_device.context()->OMSetDepthStencilState(m_dssWrite.get(), 1);
	DrawPlateFront();
	m_device.context()->ClearDepthStencilView(getDefaultRenderTarget().getDepthStencilBuffer(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_device.context()->OMSetDepthStencilState(m_dssTest.get(), 1);
	m_device.context()->RSSetState(m_rsCCW.get());

	XMFLOAT4X4 new_view;
	XMStoreFloat4x4(&new_view, XMLoadFloat4x4(&m_mirrorMtx) * m_view);
	UpdateCameraCB(new_view);

	m_cbSurfaceColor.Update(m_device.context(), WHITE_COLOR);
	DrawPuma();
	DrawWalls();
	DrawCylinder();

	m_device.context()->RSSetState(nullptr);
	//DrawMirroredParticles();

	XMFLOAT4X4 old_view;
	XMStoreFloat4x4(&old_view, m_view);
	UpdateCameraCB(old_view);
}

void Scene::DrawLight()
{
	DrawMesh(m_light, m_lightMtx);
}

void Scene::DrawPuma()
{
	m_cbSurfaceColor.Update(m_device.context(), PUMA_COLOR);
	for (size_t i = 0; i < 6; i++)
		DrawMesh(m_puma[i], m_pumaMtx[i]);
	m_cbSurfaceColor.Update(m_device.context(), WHITE_COLOR);
}

void Scene::DrawWalls()
{
	for (auto i = 0; i < 6; ++i)
	{
		m_cbSurfaceColor.Update(m_device.context(), WALLS_COLORS[i]);
		DrawMesh(m_wall, m_wallsMtx[i]);
	}
	m_cbSurfaceColor.Update(m_device.context(), WHITE_COLOR);
}

void Scene::DrawCylinder()
{
	DrawMesh(m_cylinder, m_cylinderMtx);
}

void Scene::DrawPlateFront()
{
	DrawMesh(m_plate[0], m_plateMtx[0]);
}

void Scene::DrawPlateBack()
{
	DrawMesh(m_plate[1], m_plateMtx[1]);
}

void Scene::DrawShadowVolumes()
{
	for (size_t i = 0; i < 6; i++)
		DrawMesh(m_pumaShadow[i], m_pumaMtx[i]);
}

void Scene::DrawParticles()
{
	m_device.context()->OMSetBlendState(m_bsNoDraw.get(), nullptr, BS_MASK);
	//fill z-buffer
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	DrawCylinder();
	DrawPuma();
	DrawLight();
	DrawPlateBack();
	DrawPlateFront();

	m_device.context()->OMSetBlendState(m_bsAlpha.get(), nullptr, BS_MASK);
	m_device.context()->OMSetDepthStencilState(m_dssNoWrite.get(), 0);
	m_cbWorldMtx.Update(m_device.context(), m_particleMtx);
	m_particles.Render(m_device.context());
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);

	//Particles use a geometry shader and different input layout and topology
	//which need to be reset before drawing anything else
	m_device.context()->GSSetShader(nullptr, nullptr, 0);
	m_device.context()->IASetInputLayout(m_inputlayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Scene::DrawMirroredParticles()
{
	m_device.context()->OMSetBlendState(m_bsAlpha.get(), nullptr, BS_MASK);
	m_device.context()->OMSetDepthStencilState(m_dssTestNoWrite.get(), 1);
	m_particles.Render(m_device.context());
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);

	//Particles use a geometry shader and different input layout and topology
	//which need to be reset before drawing anything else
	m_device.context()->GSSetShader(nullptr, nullptr, 0);
	m_device.context()->IASetInputLayout(m_inputlayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Scene::UpdateShadowVolume(int partIdx)
{
	vector<VertexPositionNormal> vertices;
	vector<unsigned short> indices;

	XMMATRIX invMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_pumaMtx[partIdx]));
	XMVECTOR light = XMVector3TransformCoord(XMLoadFloat4(&LIGHT_POS), invMat);

	vector<Edge> edges = GetContourEdges(partIdx, light);

	int edgeCount = edges.size();
	int tglCount = m_pumaData[partIdx].indices.size() / 3;

	vertices.reserve(edgeCount * 4 + tglCount * 3);
	indices.reserve(edgeCount * 6 + tglCount * 3);

	for (size_t i = 0; i < edgeCount; i++)
		AddVolumeTrapezoid(edges[i], light, vertices, indices);

	for (size_t i = 0; i < tglCount; i++)
		AddVolumeCupTriangle(partIdx, i, light, vertices, indices);

	m_pumaShadow[partIdx] = m_device.CreateMesh(indices, vertices);
}

vector<Edge> Scene::GetContourEdges(int partIdx, XMVECTOR &light)
{
	PumaData & part = m_pumaData[partIdx];
	vector<Edge> contour;

	int n = part.edges.size();
	for (size_t i = 0; i < n; i++)
	{
		Edge &e = part.edges[i];
		bool isFrontTop = IsFrontFaceForLight(partIdx, e.TriangleIdxTop, light);
		bool isFrontDown = IsFrontFaceForLight(partIdx, e.TriangleIdxDown, light);

		if (isFrontTop^isFrontDown) {
			if (isFrontTop) //ensure counterclockwise, 
				e.Flip();
			contour.push_back(e);
		}
	}

	return contour;
}

bool Scene::IsFrontFaceForLight(int partIdx, int tglIdx, XMVECTOR &light) {
	PumaData & part = m_pumaData[partIdx];

	XMVECTOR tglVert = XMLoadFloat3(&part.verts[part.indices[3 * tglIdx]].position);
	XMVECTOR lightVec = light - tglVert;

	XMVECTOR norm = GetTriangleNormal(partIdx, tglIdx);

	XMFLOAT3 dot;
	XMStoreFloat3(&dot, XMVector3Dot(lightVec, norm));
	bool front = dot.x >= 0; //x=y=z

	return front;
}

XMVECTOR Scene::GetTriangleNormal(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c) {
	XMVECTOR p1 = XMLoadFloat3(&a);
	XMVECTOR p2 = XMLoadFloat3(&b);
	XMVECTOR p3 = XMLoadFloat3(&c);

	XMVECTOR v = p2 - p1;
	XMVECTOR w = p3 - p1;

	XMVECTOR norm = XMVector3Normalize(XMVector3Cross(v, w));

	return norm;
}

XMVECTOR Scene::GetTriangleNormal(int partIdx, int tglIdx) {
	PumaData & part = m_pumaData[partIdx];
	XMFLOAT3 a = part.verts[part.indices[3 * tglIdx]].position;
	XMFLOAT3 b = part.verts[part.indices[3 * tglIdx + 1]].position;
	XMFLOAT3 c = part.verts[part.indices[3 * tglIdx + 2]].position;

	return GetTriangleNormal(a, b, c);
}

void Scene::AddVolumeTrapezoid(Edge &e, XMVECTOR &light, vector<VertexPositionNormal>& vertices, vector<unsigned short>& indices)
{
	XMVECTOR right = XMLoadFloat3(&e.PositionRight);
	XMVECTOR left = XMLoadFloat3(&e.PositionLeft);

	XMFLOAT3 rightBack;
	XMStoreFloat3(&rightBack, VOLUME_OFFSET * XMVector3Normalize(right - light) + right);

	XMFLOAT3 leftBack;
	XMStoreFloat3(&leftBack, VOLUME_OFFSET * XMVector3Normalize(left - light) + left);

	XMFLOAT3 normal;
	XMStoreFloat3(&normal, GetTriangleNormal(e.PositionRight, rightBack, leftBack));

	int n = vertices.size();

	vertices.push_back({ e.PositionRight, normal });
	vertices.push_back({ rightBack, normal });
	vertices.push_back({ leftBack, normal });
	vertices.push_back({ e.PositionLeft, normal });

	indices.push_back(n + 0);
	indices.push_back(n + 1);
	indices.push_back(n + 2);
	indices.push_back(n + 0);
	indices.push_back(n + 2);
	indices.push_back(n + 3);
}

void Scene::AddVolumeCupTriangle(int partIdx, int tglIdx, XMVECTOR &light, vector<VertexPositionNormal>& vertices, vector<unsigned short>& indices) {
	PumaData & part = m_pumaData[partIdx];

	int n = vertices.size();
	for (size_t i = 0; i < 3; i++)
		indices.push_back(n + i);

	VertexPositionNormal a = part.verts[part.indices[3 * tglIdx]];
	VertexPositionNormal b = part.verts[part.indices[3 * tglIdx + 1]];
	VertexPositionNormal c = part.verts[part.indices[3 * tglIdx + 2]];

	if (IsFrontFaceForLight(partIdx, tglIdx, light)) {
		vertices.push_back(a);
		vertices.push_back(b);
		vertices.push_back(c);
	}
	else {
		vertices.push_back({ MoveAlongLight(light, a.position), a.normal });
		vertices.push_back({ MoveAlongLight(light, b.position), b.normal });
		vertices.push_back({ MoveAlongLight(light, c.position), c.normal });
	}
}

XMFLOAT3 Scene::MoveAlongLight(XMVECTOR & light, XMFLOAT3 position)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMStoreFloat3(&position, VOLUME_OFFSET * XMVector3Normalize(pos - light) + pos);
	return position;
}
