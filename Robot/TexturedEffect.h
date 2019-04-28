#pragma once
#include "effect.h"
#include "constantBuffer.h"
#include <DirectXMath.h>

namespace mini
{
	namespace gk2
	{
		class TexturedEffect : public StaticEffect<BasicEffect, VSConstantBuffers, 
			PSSamplers, PSShaderResources>
		{
		public:
			enum VSConstantBufferSlots
			{
				WorldMtxSlot,
				ViewMtxSlot,
				ProjMtxSlot,
				TextureMtxSlot
			};

			enum PSSamplerSlots
			{
				TextureSamplerSlot
			};

			enum PSTextureSlots
			{
				TextureSlot
			};

			TexturedEffect() = default;

			TexturedEffect(dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11PixelShader>&& ps,
				const ConstantBuffer<DirectX::XMFLOAT4X4>& cbWorld,
				const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView, 
				const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
				const ConstantBuffer<DirectX::XMFLOAT4X4>& cbTextureMtx,
				const dx_ptr<ID3D11SamplerState>& textureSampler,
				const dx_ptr<ID3D11ShaderResourceView>& texture)
				: StaticEffect(BasicEffect(move(vs), move(ps)), 
					VSConstantBuffers{ cbWorld, cbView, cbProj, cbTextureMtx },
					PSSamplers(textureSampler), PSShaderResources(texture))
			{ }

			void SetWorldMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer)
			{
				SetVSConstantBuffer(WorldMtxSlot, buffer);
			}
			void SetViewMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& buffer)
			{
				SetVSConstantBuffer(ViewMtxSlot, buffer);
			}
			void SetProjMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer)
			{
				SetVSConstantBuffer(ProjMtxSlot, buffer);
			}
			void SetTextureMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer)
			{
				SetVSConstantBuffer(TextureMtxSlot, buffer);
			}
			void SetTextureSampler(const dx_ptr<ID3D11SamplerState>& sampler)
			{
				SetPSSampler(TextureSamplerSlot, sampler);
			}
			void SetTexture(const dx_ptr<ID3D11ShaderResourceView>& texture)
			{
				SetPSShaderResource(TextureSlot, texture);
			}
		};
	}
}
