/*------------------------------------------------------------------------*/
//								CButton.cpp
/*------------------------------------------------------------------------*/


#include "WinApp.h"
#include "Button.h"
#include "MouseMgr.h"
#include <atlstr.h>

#pragma warning(disable : 4996)

CButton::CButton()
{

}

CButton::CButton(int action, float posX, float posY, UINT16 width, UINT16 height)
{
	m_action = action;
	m_posX = posX;
	m_posY = posY;
	m_width = width;
	m_height = height;
}

CButton::~CButton()
{

}

VOID CButton::SetTexPath(char* normalPath, char* overPath, char* clickPath)
{
	m_normal_path = normalPath;
	m_over_path = overPath;
	m_click_path = clickPath;
}

VOID CButton::TextureInit(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("Texture Load Driver Error!"), TEXT("Texture Error"), MB_OK);
		return;
	}

	m_iTexture[0] = pDriver->CreateTexture(m_normal_path.c_str());
	m_iTexture[1] = pDriver->CreateTexture(m_over_path.c_str());
	m_iTexture[2] = pDriver->CreateTexture(m_click_path.c_str());
}

VOID CButton::Draw(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("Draw Button Driver Error!"), TEXT("Drawing Error"), MB_OK);
		return;
	}

	pDriver->SetTexture(m_iTexture[m_state]);
}

VOID CButton::Check(bool isWindow, bool bState, int x, int y)
{
	if (IsOnMe(x, y, isWindow))
	{
		// click
		if (bState)
		{
			m_state = click;
		}
		// click -> over
		else
		{
			m_state = over;
		}
	}
	// normal
	else
	{
		m_state = idle;
	}


}

BOOL CButton::IsOnMe(int x, int y, bool windowmode)
{
	if (windowmode)
	{
		if (x >= m_posX)
		{
			if (y >= m_posY)
			{
				if (x <= (m_posX + GetWidth()))
				{
					if (y <= (m_posY + GetHeight()))
					{
						return TRUE;
					}
				}
			}
		}
	}

	else
	{
		float ratioX = static_cast<float>(GetSystemMetrics(SM_CXSCREEN) / (float)nWidth);
		float ratioY = static_cast<float>(GetSystemMetrics(SM_CYSCREEN) / (float)nHeight);
		SetScale(ratioX, ratioY);
		if (x >= m_widePosX)
		{
			if (y >= m_widePosY)
			{
				if (x <= (m_widePosX + m_wideWidth))
				{
					if (y <= (m_widePosY + m_wideHeight))
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

VOID CButton::ButtonAction()
{

}

VOID CButton::SetScale(float scaleX, float scaleY)
{
	m_widePosX = scaleX * m_posX;
	m_widePosY = scaleY * m_posY;
	m_wideWidth = scaleX * (float)m_width;
	m_wideHeight = scaleY * (float)m_height;
	m_scaleX = scaleX;
	m_scaleY = scaleY;
}

VOID CButton::RectangleInit(CDxDriver* pDriver)
{
	memset(m_vertex, 0, sizeof(RHWVERTEX) * 4);

	POINT AnchorPoint;

	// 버튼의 정보 가져오기

	if (pDriver->WindowMode)
	{
		AnchorPoint = { nWidth / 2, nHeight / 2 };
		m_vertex[0] = { m_posX, m_posY, 1.0f, 1.0f, 0xffffffff, 0.0f, 0.0f };
		m_vertex[1] = { m_posX + m_width, m_posY, 1.0f, 1.0f, 0xffffffff, 1.0f, 0.0f };
		m_vertex[2] = { m_posX, m_posY + m_height, 1.0f, 1.0f, 0xffffffff, 0.0f, 1.0f };
		m_vertex[3] = { m_posX + m_width, m_posY + m_height, 1.0f, 1.0f, 0xffffffff, 1.0f, 1.0f };

		SetScale(1.0f, 1.0f);
	}
	else
	{
		AnchorPoint = { static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2), static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2) };
		float ratioX = static_cast<float>(GetSystemMetrics(SM_CXSCREEN) / nWidth);
		float ratioY = static_cast<float>(GetSystemMetrics(SM_CYSCREEN) / nHeight);

		m_vertex[0] = { m_posX * ratioX, m_posY * ratioY, 1.0f, 1.0f, 0xffffffff, 0.0f, 0.0f };
		m_vertex[1] = { (m_posX + m_width) * ratioX, m_posY * ratioY, 1.0f, 1.0f, 0xffffffff, 1.0f, 0.0f };
		m_vertex[2] = { m_posX * ratioX, (m_posY + m_height) * ratioY, 1.0f, 1.0f, 0xffffffff, 0.0f, 1.0f };
		m_vertex[3] = { (m_posX + m_width) * ratioX, (m_posY + m_height) * ratioY, 1.0f, 1.0f, 0xffffffff, 1.0f, 1.0f };

		SetScale(ratioX, ratioY);
	}
}