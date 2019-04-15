#pragma once
#include "camera.h"

#include <DirectXMath.h>

namespace mini
{
	class MovableCamera : public Camera
	{
	public:
		explicit MovableCamera(DirectX::XMFLOAT3 position, float angleY, float angleX);
		explicit MovableCamera(DirectX::XMFLOAT3 position);
		explicit MovableCamera();

		DirectX::XMMATRIX getViewMatrix() const override;
		DirectX::XMMATRIX GetViewMatrixInv() const;

		void Rotate(float dx, float dy);
		void Move(DirectX::XMFLOAT3 v);

		float GetXAngle() const { return m_angleX; }
		float GetYAngle() const { return m_angleY; }
		DirectX::XMFLOAT4 GetPosition() const;

		DirectX::XMVECTOR GetForwardDir() const;
		DirectX::XMVECTOR GetRightDir() const;

	private:
		void ClampRotation();

		float m_minAngle = -DirectX::XM_PIDIV2 * 8 / 9;
		float m_maxAngle = DirectX::XM_PIDIV2 * 8 / 9;
		float m_angleX, m_angleY;
		DirectX::XMFLOAT3 m_position;
	};
}