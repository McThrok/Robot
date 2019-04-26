#pragma once
#include "gk2ExampleBase.h"
#include "constantBuffer.h"
#include "mesh.h"
#include "PhongEffect.h"
#include "PhongShadowEffect.h"
#include "PumaData.h"
#include <DirectXMath.h>
#include "Edge.h"
#include "TexturedEffect.h"

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
		static const float VOLUME_OFFSET;
		static const DirectX::XMFLOAT4 MIRROR_COLOR;
		static const unsigned int BS_MASK;
		static const DirectX::XMFLOAT4 WHITE_COLOR;
		static const DirectX::XMFLOAT4 PUMA_COLOR;
		static const DirectX::XMFLOAT4 WALLS_COLORS[6];
#pragma endregion

		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbWorldMtx, //vertex shader constant buffer slot 0
			m_cbProjMtx;	//vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4X4, 2> m_cbViewMtx; //vertex shader constant buffer slot 1
		ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;	//pixel shader constant buffer slot 0
		ConstantBuffer<DirectX::XMFLOAT4> m_cbLightPos; //pixel shader constant buffer slot 1
		ConstantBuffer<DirectX::XMFLOAT4X4> m_cbMirrorTexMtx;

		double angle = 0;

		Mesh m_wall; //uses m_wallsMtx[6]
		Mesh m_plate[2]; //uses m_plateMtx
		Mesh m_puma[6]; //uses m_pumaMtx
		Mesh m_pumaShadow[6]; //uses m_pumaMtx
		PumaData m_pumaData[6]; //uses m_pumaMtx
		Mesh m_cylinder; //uses m_cylinderMtx
		
		dx_ptr<ID3D11DepthStencilState> m_dssWrite;
		dx_ptr<ID3D11DepthStencilState> m_dssTest;
		dx_ptr<ID3D11RasterizerState> m_rsCCW;
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11InputLayout> m_inputlayout;
		dx_ptr<ID3D11SamplerState> m_samplerWrap;

		DirectX::XMFLOAT4X4 m_projMtx, m_wallsMtx[6], m_plateMtx[2], m_pumaMtx[6], m_mirrorMtx, m_cylinderMtx;

		PhongEffect m_phongEffect;
		PhongShadowEffect m_phongShadowEffect;
		TexturedEffect m_mirrorTexturedEffect;

		dx_ptr<ID3D11ShaderResourceView> m_mirrorTexture;

		void UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx);
		void UpdateRobotMtx(float dt);
		void InverseKinematics(DirectX::XMVECTOR pos, DirectX::XMVECTOR normal,
			float &a1, float &a2, float &a3, float &a4, float &a5);

		void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
		void DrawMirroredWorld(DirectX::XMMATRIX m_view);
		void DrawPuma();
		void DrawWalls();
		void DrawCylinder();
		void DrawPlateFront();
		void DrawPlateBack();
		std::vector<Edge> GetContourEdges(int partIdx);
		bool IsFrontFaceForLight(int partIdx, int tglIdx);
		DirectX::XMVECTOR GetTriangleNormal(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c);
		DirectX::XMVECTOR GetTriangleNormal(int partIdx, int tglIdx);
		void UpdateShadowVolume(int partIdx);
		//std::vector<unsigned short> SortEdges(std::vector<Edge> &edges);
		void AddVolumeTrapezoid(Edge &e, std::vector<VertexPositionNormal> &vertices, std::vector<unsigned short> &indices);
	};
}
