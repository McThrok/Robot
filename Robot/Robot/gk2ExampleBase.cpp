#include "gk2ExampleBase.h"
using namespace mini;
using namespace gk2;
using namespace DirectX;

Gk2ExampleBase::Gk2ExampleBase(HINSTANCE hInstance, UINT width, UINT height, std::wstring title)
	: DxApplication(hInstance, width, height, title), m_inputDevice(hInstance),
	  m_mouse(m_inputDevice.CreateMouseDevice(m_window.getHandle())),
	  m_keyboard(m_inputDevice.CreateKeyboardDevice(m_window.getHandle())),
      m_camera(XMFLOAT3(0,0,0), 0.01f, 50.0f, 5)
{ }

bool Gk2ExampleBase::HandleCameraInput(double dt)
{
	MouseState mstate;
	if (!m_mouse.GetState(mstate))
		return false;
	auto d = mstate.getMousePositionChange();
	if (mstate.isButtonDown(0))
		m_camera.Rotate(d.y*ROTATION_SPEED, d.x*ROTATION_SPEED);
	else if (mstate.isButtonDown(1))
		m_camera.Zoom(d.y * ZOOM_SPEED);
	else
		return false;
	return true;
}

