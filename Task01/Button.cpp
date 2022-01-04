/*------------------------------------------------------------------------*/
//								CButton.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "WinApp.h"
#include "Button.h"
#include "MouseMgr.h"
#include <atlstr.h>

#pragma warning(disable : 4996)

CButton::CButton()
{

}

CButton::CButton(char* imgName_1, char* imgName_2, char* imgName_3, char* btnName)
{
	m_textureCount = 3;
	m_ppTexture = nullptr;

	LoadIniFile(imgName_1, imgName_2, imgName_3, btnName);
}



CButton::~CButton()
{
	if (m_ppTexture)
	{
		for (int i = 0; i < m_textureCount; i++)
		{
			m_ppTexture[i]->Release();
		}
		delete[] m_ppTexture;
		m_ppTexture = nullptr;
	}

}

VOID CButton::LinkD3D(CDxDriver* pDriver)
{
	g_pDriver = pDriver;
}

VOID CButton::LoadIniFile(char* imgName_1, char* imgName_2, char* imgName_3, char* btnName)
{
	// 	USES_CONVERSION;
 	m_btnName = btnName;
	m_normal_path = imgName_1;
	m_over_path = imgName_2;
	m_click_path = imgName_3;

	// ini file read
	LPCSTR szp = "\\\\ifs01\\서든어택1실\\SA_Depts\\클라이언트팀\\99. 공유\\김채원\\Study_Git\\directApp\\button.ini";
	UINT16 posX, posY, width, height;
	UINT default = -1;

	m_action = GetPrivateProfileIntA(m_btnName, "action", default, szp);
	m_posX = (FLOAT)GetPrivateProfileIntA(m_btnName, "posX", default, szp);
	m_posY = (FLOAT)GetPrivateProfileIntA(m_btnName, "posY", default, szp);
	m_width = GetPrivateProfileIntA(m_btnName, "width", default, szp);
	m_height = GetPrivateProfileIntA(m_btnName, "height", default, szp);

	SetWideInit();
}

VOID CButton::SetTexture()
{
	switch (m_state)
	{
	case idle:
	{
		m_pTexture = m_ppTexture[0];
		break;
	}

	case over:
	{
		m_pTexture = m_ppTexture[1];
		break;
	}

	case click:
	{
		m_pTexture = m_ppTexture[2];
		break;
	}
	}
}

HRESULT CButton::CreateTexture(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_ppTexture = new LPDIRECT3DTEXTURE9[m_textureCount];

	memset(m_ppTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * m_textureCount);

	CString psz = CString(m_normal_path);
	D3DXCreateTextureFromFile(pd3dDevice, psz, &m_pTexture);

	if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, psz, &m_ppTexture[0])))
	{
		MessageBox(NULL, TEXT("Button Texture 1 Load Error"), TEXT("Texture Error"), MB_OK);
		return E_FAIL;
	}

	psz = CString(m_over_path);
	if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, psz, &m_ppTexture[1])))
	{
		MessageBox(NULL, TEXT("Button Texture 2 Load Error"), TEXT("Texture Error"), MB_OK);
		return E_FAIL;
	}

	psz = CString(m_click_path);
	if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, psz, &m_ppTexture[2])))
	{
		MessageBox(NULL, TEXT("Button Texture 3 Load Error"), TEXT("Texture Error"), MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

/*
BOOL CButton::OnLButtonUp(int x, int y)
{
	BOOL bRet = FALSE;

	return bRet;
}

BOOL CButton::OnLButtonDown(int x, int y)
{
	BOOL bRet = FALSE;

	return bRet;
}
*/

BOOL CButton::IsOnMe(int x, int y, bool windowmode)
{
	// 맞음!
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


