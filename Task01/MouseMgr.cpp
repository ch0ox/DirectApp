/*------------------------------------------------------------------------*/
//								MouseMgr.cpp
/*------------------------------------------------------------------------*/


#include <windows.h>
#include "MouseMgr.h"
#include "WinApp.h"
#include "Draw.h"
#include "Button.h"
#include <map>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

CMouse::CMouse()
{
	m_MousePt.x = 0;
	m_MousePt.y = 0;
	memset(&m_mouseState, 0, sizeof(m_mouseState));
}

CMouse::~CMouse()
{

}

VOID CMouse::GetDevice()
{
	HRESULT hr = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_mouseState);
	if (FAILED(hr))
	{
		memset(&m_mouseState, 0, sizeof(m_mouseState));
		hr = m_pMouseDevice->Acquire();
	}
}

VOID CMouse::ShutDown()
{
	if (m_pMouseDevice)
	{
		m_pMouseDevice->Unacquire();
		m_pMouseDevice->Release();
		m_pMouseDevice = nullptr;
	}
}

BOOL CMouse::IsMouseDown(int button)
{
	bool b = (m_mouseState.rgbButtons[button] & 0x80) ? true : false;
	return b;
}


POINT CMouse::ClientCursorPos(HWND hWnd)
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(hWnd, &pt);

	return pt;
}
