#pragma once
#include <DirectXMath.h>
#include "effect.h"
#include <list>
#include <random>

namespace mini
{
	namespace gk2
	{
		struct ParticleVertex
		{
			DirectX::XMFLOAT3 Pos;
			float Age;
			float Angle;
			float Size;
			static const D3D11_INPUT_ELEMENT_DESC Layout[4];

			ParticleVertex() : Pos(0.0f, 0.0f, 0.0f), Age(0.0f), Angle(0.0f), Size(0.0f) { }
		};

		struct ParticleVelocities
		{
			DirectX::XMFLOAT3 Velocity;
			float AngularVelocity;

			ParticleVelocities() : Velocity(0.0f, 0.0f, 0.0f), AngularVelocity(0.0f) { }
		};

		struct Particle
		{
			ParticleVertex Vertex;
			ParticleVelocities Velocities;
		};

		class ParticleEffect : public StaticEffect<BasicEffect, GeometryShaderComponent, VSConstantBuffers, GSConstantBuffers, PSSamplers, PSShaderResources>
		{
		public:
			enum VSConstantBufferSlots
			{
				ViewMtxSlot
			};

			enum GSConstantBufferSlots
			{
				ProjMtxSlot
			};

			enum PSSamplerSlots
			{
				TextureSamplerSlot
			};

			enum PSTextureSlots
			{
				ColorMapSlot,
				OpacityMapSlot
			};

			ParticleEffect() = default;

			ParticleEffect(dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11GeometryShader>&& gs, 
				dx_ptr<ID3D11PixelShader>&& ps,	const ConstantBuffer<DirectX::XMFLOAT4X4, 2> cbView,
				const ConstantBuffer<DirectX::XMFLOAT4X4> plateView, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
				const dx_ptr<ID3D11SamplerState>& sampler, dx_ptr<ID3D11ShaderResourceView>&& colorMap,
				dx_ptr<ID3D11ShaderResourceView>&& opacityMap);

			void SetViewMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& buffer) { SetVSConstantBuffer(ViewMtxSlot, buffer); }
			void SetProjMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetGSConstantBuffer(ProjMtxSlot, buffer); }
			void SetTextureMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(ColorMapSlot, buffer); }

			void SetTextureSampler(const dx_ptr<ID3D11SamplerState>& sampler) { SetPSSampler(TextureSamplerSlot, sampler); }
			void SetColorMap(const dx_ptr<ID3D11ShaderResourceView>& texture) { SetPSShaderResource(ColorMapSlot, texture); }
			void SetOpacityMap(const dx_ptr<ID3D11ShaderResourceView>& texture) { SetPSShaderResource(OpacityMapSlot, texture); }
		};

		class ParticleSystem
		{
		public:
			ParticleSystem() = default;

			ParticleSystem(ParticleSystem&& other) = default;

			ParticleSystem(const DxDevice& device, const ConstantBuffer<DirectX::XMFLOAT4X4, 2> cbView,
				const ConstantBuffer<DirectX::XMFLOAT4X4> plateView, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
				const dx_ptr<ID3D11SamplerState>& sampler,
				DirectX::XMFLOAT3 emmiterPosition);

			ParticleSystem& operator=(ParticleSystem&& other) = default;

			void Update(const dx_ptr<ID3D11DeviceContext>& context, float dt, DirectX::XMFLOAT4 cameraPosition,
				DirectX::XMFLOAT3 emitterPos, float angle);
			void Render(const dx_ptr<ID3D11DeviceContext>& context) const;

		private:
			static const DirectX::XMFLOAT3 EMITTER_DIR;	//mean direction of particles' velocity
			static const float TIME_TO_LIVE;	//time of particle's life in seconds
			static const float EMISSION_RATE;	//number of particles to be born per second
			static const float MAX_ANGLE;		//maximal angle declination from mean direction
			static const float MIN_VELOCITY;	//minimal value of particle's velocity
			static const float MAX_VELOCITY;	//maximal value of particle's velocity
			static const float PARTICLE_SIZE;	//initial size of a particle
			static const float PARTICLE_SCALE;	//size += size*scale*dtime
			static const float MIN_ANGLE_VEL;	//minimal rotation speed
			static const float MAX_ANGLE_VEL;	//maximal rotation speed
			static const int MAX_PARTICLES;		//maximal number of particles in the system

			static const unsigned int OFFSET;
			static const unsigned int STRIDE;

			DirectX::XMFLOAT3 m_emitterPos;
			float m_particlesToCreate;
			unsigned int m_particlesCount;

			std::list<Particle> m_particles;

			dx_ptr<ID3D11Buffer> m_vertices;
			ParticleEffect m_effect;
			dx_ptr<ID3D11InputLayout> m_inputLayout;

			std::default_random_engine m_random;

			DirectX::XMFLOAT3 RandomVelocity(float angle);
			void AddNewParticle(float angle);
			static void UpdateParticle(Particle& p, float dt);
			void UpdateVertexBuffer(const dx_ptr<ID3D11DeviceContext>& context, DirectX::XMFLOAT4 cameraPosition);
		};
	}
}