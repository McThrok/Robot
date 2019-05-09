#include "MovableCamera.h"
using namespace mini;
using namespace DirectX;

MovableCamera::MovableCamera(XMFLOAT3 position, float angleY, float angleX)
	: m_angleY(angleY), m_angleX(angleX), m_position(position)
{ }

MovableCamera::MovableCamera(XMFLOAT3 position) : MovableCamera(position, 0.0f, 0.0f)
{ }

MovableCamera::MovableCamera() : MovableCamera(XMFLOAT3{ 0.0f, 0.0f, 0.0f })
{ }

void MovableCamera::ClampRotation()
{
	if (m_angleX < m_minAngle)
		m_angleX = m_minAngle;
	else if (m_angleX > m_maxAngle)
		m_angleX = m_maxAngle;
}

XMMATRIX MovableCamera::getViewMatrix() const
{
	return XMMatrixTranslation(-m_position.x, -m_position.y, -m_position.z) * XMMatrixRotationY(-m_angleY)
		* XMMatrixRotationX(-m_angleX);
}
XMMATRIX MovableCamera::GetViewMatrixInv() const
{
	return XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_angleY)
		* XMMatrixRotationX(m_angleX);
}

DirectX::XMFLOAT4 mini::MovableCamera::GetPosition() const
{
	return XMFLOAT4(m_position.x, m_position.y, m_position.z, 1);
}

XMVECTOR MovableCamera::GetForwardDir() const
{
	auto forward = XMVectorSet(0, 0, 1, 0);
	return XMVector3TransformNormal(forward, XMMatrixRotationY(GetYAngle()));
}

XMVECTOR MovableCamera::GetRightDir() const
{
	auto right = XMVectorSet(1, 0, 0, 0);
	return XMVector3TransformNormal(right, XMMatrixRotationY(GetYAngle()));
}

void MovableCamera::Rotate(float dx, float dy)
{
	m_angleX = XMScalarModAngle(m_angleX + dx);
	m_angleY = XMScalarModAngle(m_angleY + dy);
	ClampRotation();
}

void mini::MovableCamera::Move(XMFLOAT3 globalMove)
{
	m_position.y += globalMove.y;

	XMFLOAT4 globalMoveXZ = XMFLOAT4(globalMove.x, 0, globalMove.z, 0);
	XMFLOAT4 locaMove;
	XMStoreFloat4(&locaMove, XMVector3TransformNormal(XMLoadFloat4(&globalMoveXZ), XMMatrixRotationY(GetYAngle())));

	m_position.x += locaMove.x;
	m_position.z += locaMove.z;
}
