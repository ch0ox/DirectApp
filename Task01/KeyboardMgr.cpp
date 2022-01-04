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

BOOL CKeyboard::KeyDown(char key)
{
	return (m_keyboardState[key] & 0x80) != 0;
}

VOID CKeyboard::KeyManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, CDxDriver* pDriver)
{
	m_pDriver = pDriver;
	switch (wParam)
	{
	case VK_ESCAPE:
	{
		if (m_pDriver->ExitMessageBox() == IDYES)
			PostMessage(hWnd, WM_DESTROY, wParam, lParam);
		else
			return;
	}break;

	case VK_SPACE:
	{
		m_pDriver->m_d3dpp.Windowed = m_pDriver->WindowMode;

		if (m_pDriver->WindowMode)														// 현재 창모드 여서 Full Screen 으로 바꾸려고 할 때
		{
			m_pDriver->m_d3dpp.FullScreen_RefreshRateInHz = TRUE;
			m_pDriver->ChangeDisplayMode(fullScreenMode);
			m_pDriver->WindowMode = FALSE;
		}
		else
		{																	// 현재 Full Screen 이여서 창모드로 바꾸려고 할 때
			m_pDriver->m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// 창모드 일 때 0
			m_pDriver->ChangeDisplayMode(windowMode);
			m_pDriver->WindowMode = TRUE;
		}

		m_pDriver->DeviceLostRecovery();
	}break;

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