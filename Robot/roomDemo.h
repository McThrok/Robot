#pragma once
#include "gk2ExampleBase.h"
#include "constantBuffer.h"
#include "mesh.h"
#include "PhongEffect.h"

namespace mini::gk2
{
	class RoomDemo : public Gk2ExampleBase
	{
	public:
		explicit RoomDemo(HINSTANCE appInstance);

	protected:
		void Update(const Clock& dt) override;
		void Render() override;

	private:
#pragma region CONSTANTS

		static const DirectX::XMFLOAT4 LIGHT_POS;
#pragma endregion

		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbWorldMtx, //vertex shader constant buffer slot 0
			m_cbProjMtx;	//vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4X4, 2> m_cbViewMtx; //vertex shader constant buffer slot 1
		ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;	//pixel shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4> m_cbLightPos; //pixel shader constant buffer slot 1

		Mesh m_floor; //uses m_floorMtx
		Mesh m_puma; //uses m_pumaMtx

		DirectX::XMFLOAT4X4 m_projMtx, m_floorMtx, m_pumaMtx;

		dx_ptr<ID3D11RasterizerState> m_rsCullNone;
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11DepthStencilState> m_dssNoWrite;

		dx_ptr<ID3D11InputLayout> m_inputlayout;

		PhongEffect m_phongEffect;

		void UpdateCameraCB();
		void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
		void SetWorldMtx(DirectX::XMFLOAT4X4 mtx);
		void DrawScene();
	};
}