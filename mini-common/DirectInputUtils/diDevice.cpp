#include "diDevice.h"
#include "exceptions.h"

using namespace std;
using namespace mini;

DiDevice::DiDevice(HINSTANCE appInstance)
{
	Initialize(appInstance);
}

void DiDevice::Initialize(HINSTANCE appInstance)
{
	if (!appInstance)
		return;
	IDirectInput8* di;
	auto hr = DirectInput8Create(appInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		reinterpret_cast<void**>(&di), nullptr);
	m_device.reset(di);
	if (FAILED(hr))
		THROW_DX(hr);
}

di_ptr DiDevice::CreateInputDevice(HWND hWnd, const GUID& deviceGuid, const DIDATAFORMAT& dataFormat) const
{
	assert(m_device);
	IDirectInputDevice8* d;
	auto hr = m_device->CreateDevice(deviceGuid, &d, nullptr);
	di_ptr device(d);
	if (FAILED(hr))
		THROW_DX(hr);
	hr = device->SetDataFormat(&dataFormat);
	if (FAILED(hr))
		THROW_DX(hr);
	hr = device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		THROW_DX(hr);
	return device;
}
