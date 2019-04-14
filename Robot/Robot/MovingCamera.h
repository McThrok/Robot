#pragma once
#include "camera.h"

#include <DirectXMath.h>

namespace mini
{
	class MovingCamera : public Camera
	{
	public:
		explicit MovingCamera(DirectX::XMFLOAT3 position, float angleY, float angleX);
		explicit MovingCamera(DirectX::XMFLOAT3 position);
		explicit MovingCamera();

		DirectX::XMMATRIX getViewMatrix() const override;
		DirectX::XMMATRIX getViewMatrixInv() const;

		void Rotate(float dx, float dy);
		void Move(DirectX::XMFLOAT3 v);

		float getXAngle() const { return m_angleX; }
		float getYAngle() const { return m_angleY; }
		DirectX::XMFLOAT4 getPosition() const;

		/*Returns target's forward direction parallel to ground (XZ) plane*/
		DirectX::XMVECTOR getForwardDir() const;
		/*Returns target's right direction parallel to ground (XZ) plane*/
		DirectX::XMVECTOR getRightDir() const;

	private:
		void ClampRotation();

		float m_minAngle = -DirectX::XM_PIDIV2 * 8 / 9;
		float m_maxAngle = DirectX::XM_PIDIV2 * 8 / 9;
		float m_angleX, m_angleY;
		DirectX::XMFLOAT3 m_position;
	};
}