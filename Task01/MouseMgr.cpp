/*------------------------------------------------------------------------*/
//								MouseMgr.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
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
	m_btnVector.clear();
}

VOID CMouse::LinkD3D(CDxDriver* pDriver)
{
	m_pDriver = pDriver;
}

VOID CMouse::LinkButton(std::vector<CButton*>& v)
{
	m_btnVector = v;
}

constexpr int close = 0;
constexpr int left = 1;
constexpr int right = 2;

BOOL CMouse::MouseDown(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x8000) != 0;
}

VOID CMouse::MouseManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, INT flag)
{
	BOOL pressedFlag = FALSE;

	switch (flag)
	{
	case down:
	{
		if (wParam == VK_LBUTTON)
		{
			for (int state = 0; state < static_cast<int>(this->m_btnVector.size()); state++)
			{
				if (InButton(hWnd, this->m_btnVector[state]))
				{
					pressedFlag = TRUE;

					this->m_btnVector[state]->m_state = click;

					if (state == left)
						m_pDriver->pDraw->m_fAngle += 0.2f;

					else if (state == right)
						m_pDriver->pDraw->m_fAngle -= 0.2f;
				}
			}
		}

	}break;

	case up:
	{
		for (int state = 0; state < static_cast<int>(this->m_btnVector.size()); state++)
		{
			if (InButton(hWnd, this->m_btnVector[state]))
			{
				pressedFlag = FALSE;

				this->m_btnVector[state]->m_state = over;

				if (state == close)
				{
					if (m_pDriver->ExitMessageBox() == IDYES)
						PostMessage(hWnd, WM_DESTROY, wParam, lParam);
				}
				else if (state == left)
					m_pDriver->pDraw->m_fAngle += 0.2f;

				else if (state == right)
					m_pDriver->pDraw->m_fAngle -= 0.2f;
			}
		}


	}break;

	case move:
	{
		for (int state = 0; state < static_cast<int>(this->m_btnVector.size()); state++)
		{
			if (InButton(hWnd, this->m_btnVector[state]))
			{
				this->m_btnVector[state]->m_state = over;
			}
			else
				this->m_btnVector[state]->m_state = idle;
		}

	}break;
	}
}

BOOL CMouse::OnMouseMove(int x, int y)
{
	if (x == m_MousePt.x && y == m_MousePt.y)
		return TRUE;

	m_MousePt.x = x;
	m_MousePt.y = y;

	return FALSE;
}

POINT CMouse::ClientCursorPos()
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_pDriver->m_hWnd, &pt);

	return pt;
}



BOOL CMouse::InButton(HWND hWnd, CButton* pButton)
{
	POINT pt;
	pt = ClientCursorPos();

	if (pButton->IsOnMe(pt.x, pt.y, m_pDriver->WindowMode))
		return TRUE;
	else
		return FALSE;
}