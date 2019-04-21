#include <array>
#include "Scene.h"
#include "meshLoader.h"

using namespace mini;
using namespace utils;
using namespace gk2;
using namespace DirectX;
using namespace std;

const XMFLOAT4 Scene::LIGHT_POS = { 1.0f, 1.0f, 1.0f, 1.0f };

Scene::Scene(HINSTANCE appInstance) : Gk2ExampleBase(appInstance, 1280, 720, L"Robot"),
//Constant Buffers
m_cbWorldMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbProjMtx(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
m_cbViewMtx(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
m_cbLightPos(m_device.CreateConstantBuffer<XMFLOAT4>())
{
	//Projection matrix
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	m_cbProjMtx.Update(m_device.context(), m_projMtx);
	UpdateCameraCB();

	//Meshes
	vector<VertexPositionNormal> vertices;
	vector<unsigned short> indices;

	tie(vertices, indices) = MeshLoader::CreateSquare(4.0f);
	m_floor = m_device.CreateMesh(indices, vertices);

	for (size_t i = 0; i < 6; i++)
	{
		wstring path = L"resources/puma/mesh" + to_wstring(i + 1) + L".txt";
		tie(vertices, indices) = MeshLoader::LoadPumaMesh(path);
		m_puma[i] = m_device.CreateMesh(indices, vertices);
	}

	//World matrix of all objects
	XMStoreFloat4x4(&m_floorMtx, XMMatrixTranslation(0.0f, 0.0f, 2.0f) * XMMatrixRotationX(XM_PIDIV2));
	for (size_t i = 0; i < 6; i++)
	{
		XMStoreFloat4x4(&m_pumaMtx[i], XMMatrixIdentity());
	}

	//Constant buffers content
	m_cbLightPos.Update(m_device.context(), LIGHT_POS);
	m_cbSurfaceColor.Update(m_device.context(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	//Render states
	RasterizerDescription rsDesc;
	rsDesc.CullMode = D3D11_CULL_NONE;
	m_rsCullNone = m_device.CreateRasterizerState(rsDesc);

	m_bsAlpha = m_device.CreateBlendState(BlendDescription::AlphaBlendDescription());
	DepthStencilDescription dssDesc;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_dssNoWrite = m_device.CreateDepthStencilState(dssDesc);

	auto vsCode = m_device.LoadByteCode(L"phongVS.cso");
	auto psCode = m_device.LoadByteCode(L"phongPS.cso");
	m_phongEffect = PhongEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
		m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbLightPos, m_cbSurfaceColor);
	m_inputlayout = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);

	m_device.context()->IASetInputLayout(m_inputlayout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Scene::UpdateCameraCB()
{
	XMMATRIX viewMtx = m_camera.getViewMatrix();
	XMVECTOR det;
	XMMATRIX invViewMtx = XMMatrixInverse(&det, viewMtx);
	XMFLOAT4X4 view[2];
	XMStoreFloat4x4(view, viewMtx);
	XMStoreFloat4x4(view + 1, invViewMtx);
	m_cbViewMtx.Update(m_device.context(), view);
}

void Scene::Update(const Clock& c)
{
	double dt = c.getFrameTime();
	HandleCameraInput(dt);
}

void Scene::SetWorldMtx(DirectX::XMFLOAT4X4 mtx)
{
	m_cbWorldMtx.Update(m_device.context(), mtx);
}

void Scene::DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx)
{
	SetWorldMtx(worldMtx);
	m.Render(m_device.context());
}

void Scene::DrawScene()
{
	//Draw floor
	DrawMesh(m_floor, m_floorMtx);

	//Draw puma
	for (size_t i = 0; i < 6; i++)
		DrawMesh(m_puma[i], m_pumaMtx[i]);

	m_device.context()->RSSetState(nullptr);
}

void Scene::Render()
{
	Gk2ExampleBase::Render();

	getDefaultRenderTarget().Begin(m_device.context());
	m_cbProjMtx.Update(m_device.context(), m_projMtx);
	UpdateCameraCB();

	m_phongEffect.Begin(m_device.context());
	DrawScene();
}