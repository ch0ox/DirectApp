/*------------------------------------------------------------------------*/
//								KeyboardMgr.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
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

VOID CKeyboard::InitializeKeyboard()
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

BOOL CKeyboard::KeyDown(char key)
{
	return (m_keyboardState[key] & 0x80) != 0;
}

VOID CKeyboard::KeySpace(CDxDriver* pDriver)
{
	pDriver->m_d3dpp.Windowed = pDriver->WindowMode;

	if (pDriver->WindowMode)														// 현재 창모드 여서 Full Screen 으로 바꾸려고 할 때
	{
		pDriver->m_d3dpp.FullScreen_RefreshRateInHz = TRUE;
		pDriver->ChangeDisplayMode(fullScreenMode);
		pDriver->WindowMode = FALSE;
	}
	else
	{																	// 현재 Full Screen 이여서 창모드로 바꾸려고 할 때
		pDriver->m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// 창모드 일 때 0
		pDriver->ChangeDisplayMode(windowMode);
		pDriver->WindowMode = TRUE;
	}

	pDriver->DeviceLostRecovery();
}

VOID CKeyboard::KeyManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, CDxDriver* pDriver)
{
	m_pDriver = pDriver;
	switch (wParam)
	{
	case VK_LEFT:
	{
		m_pDriver->pDraw->m_fAngle += 0.2f;
	}break;
	case VK_RIGHT:
	{
		m_pDriver->pDraw->m_fAngle -= 0.2f;
	}break;
	}
}