#include "MovingCamera.h"

using namespace mini;
using namespace DirectX;

MovingCamera::MovingCamera(XMFLOAT3 position, float angleY, float angleX)
	: m_angleY(angleY), m_angleX(angleX), m_position(position)
{ }

MovingCamera::MovingCamera(XMFLOAT3 position) : MovingCamera(position, 0.0f, 0.0f)
{ }

MovingCamera::MovingCamera() : MovingCamera(XMFLOAT3{ 0.0f, 0.0f, 0.0f })
{ }

void MovingCamera::ClampRotation()
{
	if (m_angleX < m_minAngle)
		m_angleX = m_minAngle;
	else if (m_angleX > m_maxAngle)
		m_angleX = m_maxAngle;
}

XMMATRIX MovingCamera::getViewMatrix() const
{
	return XMMatrixTranslation(-m_position.x, -m_position.y, -m_position.z) * XMMatrixRotationY(-m_angleY)
		* XMMatrixRotationX(-m_angleX);
}
XMMATRIX MovingCamera::getViewMatrixInv() const
{
	return XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_angleY)
		* XMMatrixRotationX(m_angleX);
}

DirectX::XMFLOAT4 mini::MovingCamera::getPosition() const
{
	return XMFLOAT4(m_position.x, m_position.y, m_position.z, 1);
}

XMVECTOR MovingCamera::getForwardDir() const
{
	auto forward = XMVectorSet(0, 0, 1, 0);
	return XMVector3TransformNormal(forward, XMMatrixRotationY(getYAngle()));
}

XMVECTOR MovingCamera::getRightDir() const
{
	auto right = XMVectorSet(1, 0, 0, 0);
	return XMVector3TransformNormal(right, XMMatrixRotationY(getYAngle()));
}

void MovingCamera::Rotate(float dx, float dy)
{
	m_angleX = XMScalarModAngle(m_angleX + dx);
	m_angleY = XMScalarModAngle(m_angleY + dy);
	ClampRotation();
}

void mini::MovingCamera::Move(float dx, float dy, float dz)
{
	m_position.x += dx;
	m_position.y += dy;
	m_position.z += dz;
}

