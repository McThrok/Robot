#pragma once
#include "dxApplication.h"
#include "diDevice.h"
#include "keyboard.h"
#include "mouse.h"
#include "MovingCamera.h"

namespace mini::gk2
{
	class Gk2ExampleBase : public DxApplication
	{
	public:
		Gk2ExampleBase(HINSTANCE hInstance, UINT width, UINT height, std::wstring title);

	protected:
		void HandleCameraInput(double dt);

		DiDevice m_inputDevice;
		Mouse m_mouse;
		Keyboard m_keyboard;
		MovingCamera m_camera;

	private:
		static constexpr float ROTATION_SPEED = 0.002f;
		static constexpr float MOVEMENT_SPEED = 2.0;

		static constexpr BYTE KEY_W = 17;
		static constexpr BYTE KEY_A = 30;
		static constexpr BYTE KEY_S = 31;
		static constexpr BYTE KEY_D = 32;
	};
}

