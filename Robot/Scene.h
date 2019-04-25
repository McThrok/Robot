#pragma once
#include "gk2ExampleBase.h"
#include "constantBuffer.h"
#include "mesh.h"
#include "PhongEffect.h"

namespace mini::gk2
{
	class Scene : public Gk2ExampleBase
	{
	public:
		explicit Scene(HINSTANCE appInstance);
		void CreateRenderStates();

	protected:
		void Update(const Clock& dt) override;
		void Render() override;

	private:
#pragma region CONSTANTS
		static const DirectX::XMFLOAT4 LIGHT_POS;
		static const DirectX::XMFLOAT4 MIRROR_COLOR;
		static const unsigned int BS_MASK;
#pragma endregion

		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbWorldMtx, //vertex shader constant buffer slot 0
			m_cbProjMtx;	//vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4X4, 2> m_cbViewMtx; //vertex shader constant buffer slot 1
		ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;	//pixel shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4> m_cbLightPos; //pixel shader constant buffer slot 1

		double angle = 0;

		Mesh m_floor; //uses m_floorMtx
		Mesh m_plate[2]; //uses m_plateMtx
		Mesh m_puma[6]; //uses m_pumaMtx
		
		dx_ptr<ID3D11DepthStencilState> m_dssWrite;
		dx_ptr<ID3D11DepthStencilState> m_dssTest;
		dx_ptr<ID3D11RasterizerState> m_rsCCW;
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11InputLayout> m_inputlayout;

		DirectX::XMFLOAT4X4 m_projMtx, m_floorMtx, m_plateMtx[2], m_pumaMtx[6];
		DirectX::XMFLOAT4X4 m_mirrorMtx;

		PhongEffect m_phongEffect;

		void UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx);
		void UpdateRobotMtx(float dt);
		void InverseKinematics(DirectX::XMVECTOR pos, DirectX::XMVECTOR normal, 
			float &a1, float &a2, float &a3, float &a4, float &a5);
		
		void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
		void DrawMirroredWorld(DirectX::XMMATRIX m_view);
		void DrawPuma();
		void DrawFloor();
		void DrawPlateFront();
		void DrawPlateBack();
	};
}
