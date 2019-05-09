#include "gk2ExampleBase.h"
using namespace mini;
using namespace gk2;
using namespace DirectX;

Gk2ExampleBase::Gk2ExampleBase(HINSTANCE hInstance, UINT width, UINT height, std::wstring title)
	: DxApplication(hInstance, width, height, title), m_inputDevice(hInstance),
	m_mouse(m_inputDevice.CreateMouseDevice(m_window.getHandle())),
	m_keyboard(m_inputDevice.CreateKeyboardDevice(m_window.getHandle())),
	m_camera(XMFLOAT3{ 0.0f, 0.0f, -10.0f })
{ }

void Gk2ExampleBase::HandleCameraInput(double dt)
{
	MouseState mstate;
	if (m_mouse.GetState(mstate))
		if (mstate.isButtonDown(0)) {
			POINT posChange = mstate.getMousePositionChange();
			m_camera.Rotate(posChange.y*ROTATION_SPEED, posChange.x*ROTATION_SPEED);
		}

	KeyboardState kstate;
	if (m_keyboard.GetState(kstate)) {
		XMFLOAT3 moveVec = XMFLOAT3(0, 0, 0);

		if (kstate.isKeyDown(KEY_Q))
			moveVec.y -= MOVEMENT_SPEED * dt;

		if (kstate.isKeyDown(KEY_W))
			moveVec.z += MOVEMENT_SPEED * dt;

		if (kstate.isKeyDown(KEY_A))
			moveVec.x -= MOVEMENT_SPEED * dt;

		if (kstate.isKeyDown(KEY_S))
			moveVec.z -= MOVEMENT_SPEED * dt;

		if (kstate.isKeyDown(KEY_D))
			moveVec.x += MOVEMENT_SPEED * dt;

		if (kstate.isKeyDown(KEY_E))
			moveVec.y += MOVEMENT_SPEED * dt;

		m_camera.Move(moveVec);
	}
}

