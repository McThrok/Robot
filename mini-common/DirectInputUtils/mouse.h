#pragma once

#include "deviceBase.h"
#include "mouseState.h"

namespace mini
{
	class Mouse : public DeviceBase
	{
	public:
		bool GetState(MouseState& state) const;

		explicit Mouse(di_ptr&& device = nullptr)
			: DeviceBase(std::move(device))
		{ }

		Mouse(Mouse&& other)
			: DeviceBase(std::move(other.m_device))
		{ }

		Mouse(const Mouse& other) = delete;

		Mouse& operator=(Mouse&& other)
		{
			this->DeviceBase::operator=(std::move(other));
			return *this;
		}

		Mouse& operator=(const Mouse& other) = delete;
	};
}