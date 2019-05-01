#pragma once
#include <DirectXMath.h>
#include "gk2ExampleBase.h"
#include "constantBuffer.h"
#include "mesh.h"
#include "PumaData.h"
#include "PhongEffect.h"
#include "TexturedEffect.h"
#include "ParticleSystem.h"
using namespace DirectX;
using namespace std;

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

		static const XMFLOAT4 LIGHT_POS;
		static const float CLEAR_COLOR[4];
		static const float VOLUME_OFFSET;
		static const XMFLOAT4 MIRROR_COLOR;
		static const unsigned int BS_MASK;
		static const XMFLOAT4 BLACK_COLOR;
		static const XMFLOAT4 WHITE_COLOR;
		static const XMFLOAT4 PUMA_COLOR;
		static const XMFLOAT4 WALLS_COLORS[6];
#pragma endregion

		ConstantBuffer<XMFLOAT4X4> m_cbWorldMtx, //vertex shader constant buffer slot 0
			m_cbProjMtx;	//vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0
		ConstantBuffer<XMFLOAT4X4, 2> m_cbViewMtx; //vertex shader constant buffer slot 1
		ConstantBuffer<XMFLOAT4> m_cbSurfaceColor;	//pixel shader constant buffer slot 0
		ConstantBuffer<XMFLOAT4> m_cbLightColor;	//pixel shader constant buffer slot 0
		ConstantBuffer<XMFLOAT4> m_cbLightPos; //pixel shader constant buffer slot 1
		ConstantBuffer<XMFLOAT4X4> m_cbMirrorTexMtx;

		double angle = 0;

		Mesh m_wall; //uses m_wallsMtx[6]
		Mesh m_light; //uses m_lightMtx[6]
		Mesh m_plate[2]; //uses m_plateMtx
		Mesh m_puma[6]; //uses m_pumaMtx
		Mesh m_pumaShadow[6]; //uses m_pumaMtx
		PumaData m_pumaData[6]; //uses m_pumaMtx
		Mesh m_cylinder; //uses m_cylinderMtx

		dx_ptr<ID3D11BlendState> m_bsNoDraw;
		dx_ptr<ID3D11DepthStencilState> m_dssInitShadow;
		dx_ptr<ID3D11RasterizerState> m_rsInitShadow;
		dx_ptr<ID3D11DepthStencilState> m_dssRenderShadow;
		dx_ptr<ID3D11DepthStencilState> m_dssRenderNoShadow;
		dx_ptr<ID3D11DepthStencilState> m_dssWrite, m_dssNoWrite, m_dssTestNoWrite;
		dx_ptr<ID3D11DepthStencilState> m_dssTest;
		dx_ptr<ID3D11RasterizerState> m_rsCCW;
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11InputLayout> m_inputlayout;
		dx_ptr<ID3D11SamplerState> m_samplerWrap;

		XMFLOAT4X4 m_projMtx, m_wallsMtx[6], m_plateMtx[2], m_pumaMtx[6], m_mirrorMtx, m_cylinderMtx, m_lightMtx;

		PhongEffect m_phongEffect;
		TexturedEffect m_mirrorTexturedEffect;
		ParticleSystem m_particles;

		dx_ptr<ID3D11ShaderResourceView> m_mirrorTexture;

		void UpdateCameraCB(XMFLOAT4X4 cameraMtx);
		void UpdateRobotMtx(float dt);
		void UpdateParticles(float dt);
		void InverseKinematics(XMVECTOR pos, XMVECTOR normal,
			float &a1, float &a2, float &a3, float &a4, float &a5);

		void DrawMesh(const Mesh& m, XMFLOAT4X4 worldMtx);
		void DrawMirroredWorld(XMMATRIX m_view);
		void DrawLight();
		void DrawPuma();
		void DrawWalls();
		void DrawCylinder();
		void DrawPlateFront();
		void DrawPlateBack();
		void DrawShadowVolumes();
		void DrawMirroredParticles();
		void DrawParticles();

		void RenderScene();
		void RenderMirror(XMMATRIX m_view);
		void FillStencilShadows();

		void UpdateShadowVolume(int partIdx);
		vector<Edge> GetContourEdges(int partIdx, XMVECTOR &light);
		bool IsFrontFaceForLight(int partIdx, int tglIdx, XMVECTOR &light);
		XMVECTOR GetTriangleNormal(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c);
		XMVECTOR GetTriangleNormal(int partIdx, int tglIdx);
		void AddVolumeTrapezoid(Edge &e, XMVECTOR &light, vector<VertexPositionNormal> &vertices, vector<unsigned short> &indices);
		void AddVolumeCupTriangle(int partIdx, int tglIdx, XMVECTOR &light, vector<VertexPositionNormal>& vertices, vector<unsigned short>& indices);
		XMFLOAT3 MoveAlongLight(XMVECTOR & light, XMFLOAT3 position);
	};
}
