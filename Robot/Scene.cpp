#include <array>
#include "Scene.h"
#include "meshLoader.h"

using namespace mini;
using namespace utils;
using namespace gk2;
using namespace DirectX;
using namespace std;

const XMFLOAT4 Scene::LIGHT_POS = { 1.0f, 1.0f, 1.0f, 1.0f };
const unsigned int Scene::BS_MASK = 0xffffffff;

Scene::Scene(HINSTANCE appInstance) : Gk2ExampleBase(appInstance, 1280, 720, L"Robot"),
//Constant Buffers
m_cbWorldMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbProjMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()), 
m_cbMirrorTexMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbViewMtx(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
m_cbLightPos(m_device.CreateConstantBuffer<XMFLOAT4>()),

//Textures
m_mirrorTexture(m_device.CreateShaderResourceView(L"resources/textures/mirror_texture.png"))
{
	//Projection matrix
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	m_cbProjMtx.Update(m_device.context(), m_projMtx);

	CreateRenderStates();

	//Meshes
	vector<VertexPositionNormal> vertices;
	vector<unsigned short> indices;

	// Floor
	tie(vertices, indices) = MeshLoader::CreateSquare(4.0f);
	m_floor = m_device.CreateMesh(indices, vertices);

	XMStoreFloat4x4(&m_floorMtx, XMMatrixTranslation(0.0f, 0.0f, 1.0f)
		* XMMatrixRotationX(XM_PIDIV2));

	// Plate
	tie(vertices, indices) = MeshLoader::CreateSquare(1.5f);
	for (size_t i = 0; i < 2; i++)
		m_plate[i] = m_device.CreateMesh(indices, vertices);

	XMStoreFloat4x4(&m_plateMtx[0], XMMatrixRotationY(-XM_PI / 3)
		* XMMatrixRotationX(XM_PIDIV2)
		* XMMatrixTranslation(-1.5f, 0.2f, 0.0f));
	XMStoreFloat4x4(&m_plateMtx[1], XMMatrixRotationY(2 * XM_PI / 3)
		* XMMatrixRotationX(XM_PIDIV2)
		* XMMatrixTranslation(-1.5f, 0.2f, 0.0f));

	XMMATRIX m_scale = XMMatrixScaling(1.0f, 1.0f, -1.0f);
	XMMATRIX m = XMLoadFloat4x4(&m_plateMtx[0]);
	XMMATRIX m_inverse = XMMatrixInverse(nullptr, m);
	XMStoreFloat4x4(&m_mirrorMtx, m_inverse * m_scale * m);

	// Puma
	for (size_t i = 0; i < 6; i++)
	{
		wstring path = L"resources/puma/mesh" + to_wstring(i + 1) + L".txt";
		tie(vertices, indices) = MeshLoader::LoadPumaMesh(path);
		m_puma[i] = m_device.CreateMesh(indices, vertices);
	}

	for (size_t i = 0; i < 6; i++)
		XMStoreFloat4x4(&m_pumaMtx[i], XMMatrixIdentity());

	// Phong
	auto vsCode = m_device.LoadByteCode(L"phongVS.cso");
	auto psCode = m_device.LoadByteCode(L"phongPS.cso");

	m_phongEffect = PhongEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
		m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbLightPos, m_cbSurfaceColor);
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
	
	m_texturedEffect = TexturedEffect(m_device.CreateVertexShader(vsCode),
		m_device.CreatePixelShader(psCode),
		m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbMirrorTexMtx, m_samplerWrap, m_mirrorTexture);

	//Constant buffers content
	m_cbLightPos.Update(m_device.context(), LIGHT_POS);

	m_device.context()->IASetInputLayout(m_inputlayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mini::gk2::Scene::CreateRenderStates()
{
	DepthStencilDescription dssDesc;
	m_dssWrite = m_device.CreateDepthStencilState(dssDesc.StencilWriteDescription());
	m_dssTest = m_device.CreateDepthStencilState(dssDesc.StencilTestDescription());
	m_rsCCW = m_device.CreateRasterizerState(RasterizerDescription(true));

	BlendDescription bsDesc;
	m_bsAlpha = m_device.CreateBlendState(bsDesc.AlphaBlendDescription());
}

void mini::gk2::Scene::UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx)
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
	HandleCameraInput(dt);
	UpdateRobotMtx(dt);
}

void mini::gk2::Scene::UpdateRobotMtx(float dt)
{
	angle += dt;
	XMVECTOR axis = { sqrt(3), 1, 0 };
	XMVECTOR pos = XMVector3Transform({ 0, 0, 0, 1 }, XMMatrixTranslation(0.0f, 0.0f, 0.5f)
		* XMMatrixRotationAxis(axis, angle) * XMMatrixTranslation(-1.5f, 0.2f, 0.0f));
	float a1, a2, a3, a4, a5;
	InverseKinematics(pos, axis, a1, a2, a3, a4, a5);

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

void mini::gk2::Scene::InverseKinematics(XMVECTOR pos, XMVECTOR normal,
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
	DrawMirroredWorld(m_view);
	
	XMFLOAT4X4 old_view;
	XMStoreFloat4x4(&old_view, m_view);
	UpdateCameraCB(old_view);

	m_device.context()->OMSetBlendState(m_bsAlpha.get(), nullptr, BS_MASK);
	m_texturedEffect.SetTexture(m_mirrorTexture);
	m_texturedEffect.Begin(m_device.context());
	DrawMesh(m_plate[0], m_plateMtx[0]);
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);

	m_cbSurfaceColor.Update(m_device.context(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	m_phongEffect.Begin(m_device.context());

	DrawPuma();
	DrawFloor();
	DrawPlateBack();
}

void mini::gk2::Scene::DrawMirroredWorld(XMMATRIX m_view)
{
	m_device.context()->OMSetDepthStencilState(m_dssWrite.get(), 1);
	DrawPlateFront();
	m_device.context()->OMSetDepthStencilState(m_dssTest.get(), 1);
	m_device.context()->RSSetState(m_rsCCW.get());

	XMFLOAT4X4 new_view;
	XMStoreFloat4x4(&new_view, XMLoadFloat4x4(&m_mirrorMtx) * m_view);
	UpdateCameraCB(new_view);

	m_cbSurfaceColor.Update(m_device.context(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	DrawPuma();
	DrawFloor();
	
	m_device.context()->RSSetState(nullptr);
	m_device.context()->OMSetDepthStencilState(nullptr, 0);
}

void Scene::DrawPuma()
{
	for (size_t i = 0; i < 6; i++)
		DrawMesh(m_puma[i], m_pumaMtx[i]);
}

void Scene::DrawFloor()
{
	DrawMesh(m_floor, m_floorMtx);
}

void Scene::DrawPlateFront()
{
	DrawMesh(m_plate[0], m_plateMtx[0]);
}

void Scene::DrawPlateBack()
{
	DrawMesh(m_plate[1], m_plateMtx[1]);
}
