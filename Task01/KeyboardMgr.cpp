/*------------------------------------------------------------------------*/
//								KeyboardMgr.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include <windows.h>
#include "KeyboardMgr.h"
#include "WinApp.h"
#include "Draw.h"

CKeyboard::CKeyboard()
{

}

CKeyboard::~CKeyboard()
{

}

VOID CKeyboard::KeyManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, CDxDriver* pDriver)
{
	g_pDriver = pDriver;
	switch (wParam)
	{
	case VK_ESCAPE:
	{
		if (g_pDriver->ExitMessageBox() == IDYES)
			PostMessage(hWnd, WM_DESTROY, wParam, lParam);
		else
			return;
	}break;

	case VK_SPACE:
	{
		g_pDriver->d3dpp.Windowed = g_pDriver->WindowMode;

		if (g_pDriver->WindowMode)														// ���� â��� ���� Full Screen ���� �ٲٷ��� �� ��
		{
			g_pDriver->d3dpp.FullScreen_RefreshRateInHz = TRUE;
			g_pDriver->ChangeDisplayMode(fullScreenMode);
			g_pDriver->WindowMode = FALSE;
		}
		else
		{																	// ���� Full Screen �̿��� â���� �ٲٷ��� �� ��
			g_pDriver->d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// â��� �� �� 0
			g_pDriver->ChangeDisplayMode(windowMode);
			g_pDriver->WindowMode = TRUE;
		}

		g_pDriver->DeviceLostRecovery();
	}break;

	case VK_LEFT:
	{
		g_pDriver->pDraw->m_fAngle += 0.2f;
	}break;
	case VK_RIGHT:
	{
		g_pDriver->pDraw->m_fAngle -= 0.2f;
	}break;
	}
}