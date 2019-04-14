#pragma once

#include "diptr.h"

namespace mini
{
	class DeviceBase
	{
	public:
		static const unsigned int GET_STATE_RETRIES = 2;
		static const unsigned int AQUIRE_RETRIES = 2;

		DeviceBase(const DeviceBase& other) = delete;
		DeviceBase& operator=(const DeviceBase& other) = delete;

	protected:
		explicit DeviceBase(di_ptr&& device)
			: m_device(std::move(device))
		{ }

		DeviceBase(DeviceBase&& other)
			: m_device(std::move(other.m_device))
		{ }

		DeviceBase& operator=(DeviceBase&& other)
		{
			m_device = move(other.m_device);
			return *this;
		}

		bool GetState(unsigned int size, void* ptr) const;

		di_ptr m_device;
	};
}