#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include <cassert>

struct MouseState
{
	enum Buttons
	{
		Left = 0,
		Right = 1,
		Middle = 2
	};

	static const BYTE BUTTON_MASK = 0x80;

	DIMOUSESTATE m_state;

	MouseState()
	{
		ZeroMemory(&m_state, sizeof(DIMOUSESTATE));
	}

	MouseState(const MouseState& other) = default;

	MouseState& operator=(const MouseState& other) = default;

	POINT getMousePositionChange() const
	{
		POINT r;
		r.x = m_state.lX;
		r.y = m_state.lY;
		return r;
	}

	LONG getWheelPositionChange() const
	{
		return m_state.lZ;
	}

	bool isButtonDown(BYTE button) const
	{
		assert(button < 4);
		return 0 != (m_state.rgbButtons[button] & BUTTON_MASK);
	}

	bool isButtonUp(BYTE button) const
	{
		assert(button < 4);
		return 0 == (m_state.rgbButtons[button] & BUTTON_MASK);
	}

	bool operator[](BYTE button) const
	{
		assert(button < 4);
		return 0 != (m_state.rgbButtons[button] & BUTTON_MASK);
	}
};
