#pragma once
#include "effect.h"
#include "constantBuffer.h"
#include <DirectXMath.h>

namespace mini
{
	namespace gk2
	{
		class PhongShadowEffect : public StaticEffect<BasicEffect, GeometryShaderComponent, VSConstantBuffers, GSConstantBuffers, PSConstantBuffers>
		{
		public:

			//enumerated types defined for convenience
			//enum ConstantBufferSlots
			//{
			//	WorldMtxSlot,
			//	ViewMtxSlot,
			//	ProjMtxSlot,
			//	LightPositionSlot,
			//	SurfaceColorSlot
			//};

			PhongShadowEffect() = default;

			PhongShadowEffect(dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11GeometryShader>&& gs, dx_ptr<ID3D11PixelShader>&& ps,
				const ConstantBuffer<DirectX::XMFLOAT4X4>& cbWorld,
				const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
				const ConstantBuffer<DirectX::XMFLOAT4>& cbLightPos, const ConstantBuffer<DirectX::XMFLOAT4>& cbSurfaceColor)
				: StaticEffect(BasicEffect{ move(vs), move(ps) }, GeometryShaderComponent(move(gs)),
					VSConstantBuffers{ cbWorld }, GSConstantBuffers{ cbWorld, cbView, cbProj, cbLightPos }, PSConstantBuffers{ cbView, cbLightPos, cbSurfaceColor })
			{ }

			//setter methods defined for convenience
			//void SetWorldMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(WorldMtxSlot, buffer); }
			//void SetViewMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& buffer) { SetVSConstantBuffer(ViewMtxSlot, buffer); }
			//void SetProjMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(ProjMtxSlot, buffer); }
			//void SetLightPosBuffer(const ConstantBuffer<DirectX::XMFLOAT4>& buffer) { SetPSConstantBuffer(LightPositionSlot, buffer); }
			//void SetSurfaceColorBuffer(const ConstantBuffer<DirectX::XMFLOAT4>& buffer) { SetPSConstantBuffer(SurfaceColorSlot, buffer); }
		};
	}
}
