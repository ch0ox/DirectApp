/*------------------------------------------------------------------------*/
//								KeyboardMgr.cpp
/*------------------------------------------------------------------------*/


#include <windows.h>
#include "KeyboardMgr.h"
#include "WinApp.h"
#include "Draw.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

CKeyboard::CKeyboard()
{
	memset(&m_keyboardState, 0, sizeof(char) * 256);
}

CKeyboard::~CKeyboard()
{

}

VOID CKeyboard::GetDevice()
{
	HRESULT hr = m_pKeyDevice->GetDeviceState(sizeof(m_keyboardState), (void**)&m_keyboardState);
	if (FAILED(hr))
	{
		memset(&m_keyboardState, 0, sizeof(m_keyboardState));
		hr = m_pKeyDevice->Acquire();
	}
}

VOID CKeyboard::ShutDown()
{
	if (m_pKeyDevice)
	{
		m_pKeyDevice->Unacquire();
		m_pKeyDevice->Release();
		m_pKeyDevice = nullptr;
	}
}

BOOL CKeyboard::IsKeyDown(char key)
{
	bool b = (m_keyboardState[key] & 0x80) ? true : false;
	return b;
}

VOID CKeyboard::Toggle(CDxDriver* pDriver)
{
	pDriver->m_d3dpp.Windowed = pDriver->WindowMode;

	if (pDriver->WindowMode)														// ���� â��� ���� Full Screen ���� �ٲٷ��� �� ��
	{
		pDriver->m_d3dpp.FullScreen_RefreshRateInHz = TRUE;
		pDriver->ChangeDisplayMode(fullScreenMode);
		pDriver->WindowMode = FALSE;
	}
	else
	{																	// ���� Full Screen �̿��� â���� �ٲٷ��� �� ��
		pDriver->m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// â��� �� �� 0
		pDriver->ChangeDisplayMode(windowMode);
		pDriver->WindowMode = TRUE;
	}

	pDriver->DeviceLostRecovery();
}