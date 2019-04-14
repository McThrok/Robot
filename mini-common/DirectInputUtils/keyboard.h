#pragma once
#include "deviceBase.h"
#include "keyboardState.h"

namespace mini
{
	class Keyboard : public DeviceBase
	{
	public:
		bool GetState(KeyboardState& state) const;

		explicit Keyboard(di_ptr&& device = nullptr)
			: DeviceBase(std::move(device))
		{ }

		Keyboard(Keyboard&& other)
			: DeviceBase(std::move(other.m_device))
		{ }

		Keyboard(const Keyboard& other) = delete;

		Keyboard& operator=(Keyboard&& other)
		{
			this->DeviceBase::operator=(std::move(other));
			return *this;
		}

		Keyboard& operator=(const Keyboard& other) = delete;
	};
}