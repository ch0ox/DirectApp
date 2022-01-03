/*------------------------------------------------------------------------*/
//								CButton.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "WinApp.h"
#include "Button.h"
#include "MouseMgr.h"
#include "Cini.h"
#include <atlstr.h>

#pragma warning(disable : 4996)

CButton::CButton()
{
	g_pDriver = nullptr;
	m_state = 0;
	m_scaleX = 1.0f;
	m_scaleY = 1.0f;

	m_posX = 0;
	m_posY = 0;
	m_width = 0;
	m_height = 0;

	m_widePosX = 0;
	m_widePosY = 0;
	m_wideWidth = 0;
	m_wideHeight = 0;

	m_btnName = { 0, };
	m_path = { 0, };
	m_normal_path = { 0, };
	m_over_path = { 0, };
	m_click_path = { 0, };

	m_textureCount = 0;
	m_pTexture = nullptr;
	m_ppTexture = nullptr;
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
	// ini 파일 경로 변경 설정
	wchar_t szPathName[MAX_PATH] = TEXT("\\\\ifs01\\서든어택1실\\SA_Depts\\클라이언트팀\\99. 공유\\김채원\\Task03_D\\button.ini"); 
	CIni iniFile(szPathName);
	m_btnName = btnName;

	USES_CONVERSION;
	CString psz = CString(m_btnName);

	//TCHAR path[MAX_PATH] = { 0, };
	//LPWSTR path2 = NULL;

	m_posX = (FLOAT)iniFile.GetDouble(psz, TEXT("posX"), 0);						// pos X
	m_posY = (FLOAT)iniFile.GetDouble(psz, TEXT("posY"), 0);						// pos Y
	m_width = (UINT16)iniFile.GetInt(psz, TEXT("width"), 0);						// width
	m_height = (UINT16)iniFile.GetInt(psz, TEXT("height"), 0);						// height

	m_normal_path = imgName_1;
	m_over_path = imgName_2;
	m_click_path = imgName_3;

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


