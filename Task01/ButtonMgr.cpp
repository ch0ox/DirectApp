#include "WinApp.h"
#include "Button.h"
#include "ButtonMgr.h"
#include "MouseMgr.h"
#include <atlstr.h>
#include <tchar.h>
#include <cstdio>

CButtonMgr::CButtonMgr()
{

}

CButtonMgr::~CButtonMgr()
{
	m_btnIter = m_btnVector.begin();

	for (; m_btnIter != m_btnVector.end(); ++m_btnIter)
	{
		delete (*m_btnIter);
		*m_btnIter = nullptr;
	}
}

BOOL CButtonMgr::LoadIniFile(char* szFilePath)
{
	char btnName[64] = { 0, };
	char szTmp[4096] = { 0, };
	UINT action = -1;
	FLOAT scaleX = 1.0f;
	FLOAT scaleY = 1.0f;
	FLOAT posX = 0;
	FLOAT posY = 0;
	UINT16 width = 0;
	UINT16 height = 0;

	char normal_path[MAX_PATH] = { 0, };
	char over_path[MAX_PATH] = { 0, };
	char click_path[MAX_PATH] = { 0, };

	// ini file read
	LPCSTR szp = szFilePath;
	UINT default = (UINT)-1;
	int cnt = 0;
	CButton* pBtn = nullptr;

	while (true)
	{
		sprintf(btnName, "Button%d", cnt);
		DWORD dw = GetPrivateProfileSectionA(btnName, szTmp, 4096, szp);
		if (dw <= 0)
			break;

		action = GetPrivateProfileIntA(btnName, "action", default, szp);
		posX = (FLOAT)GetPrivateProfileIntA(btnName, "posX", default, szp);
		posY = (FLOAT)GetPrivateProfileIntA(btnName, "posY", default, szp);
		width = GetPrivateProfileIntA(btnName, "width", default, szp);
		height = GetPrivateProfileIntA(btnName, "height", default, szp);

		GetPrivateProfileStringA(btnName, "NormalPath", "", normal_path, MAX_PATH, szp);
		GetPrivateProfileStringA(btnName, "OverPath", "", over_path, MAX_PATH, szp);
		GetPrivateProfileStringA(btnName, "ClickPath", "", click_path, MAX_PATH, szp);

		pBtn = new CButton(action, posX, posY, width, height);
		pBtn->SetTexPath(normal_path, over_path, click_path);
		m_btnVector.push_back(pBtn);

		cnt++;
	}

	return true;
}

VOID CButtonMgr::Initialize(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}

	for (int i = 0; i < m_btnVector.size(); i++)
	{
		m_btnVector[i]->RectangleInit(pDriver);
		m_btnVector[i]->TextureInit(pDriver);
	}
}

VOID CButtonMgr::Check(CDxDriver* pDriver, CDxInput* pInput, bool bState, int x, int y)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}
	if (pInput == nullptr)
	{
		MessageBox(NULL, TEXT("No input device."), TEXT("Error"), MB_OK);
		return;
	}

	for (int i = 0; i < m_btnVector.size(); i++)
	{
		m_btnVector[i]->Check(pDriver->WindowMode, bState, x, y);
		if (m_btnVector[i]->GetState() == click)
		{
			Action(pDriver, pInput, m_btnVector[i]->GetAction());
		}
	}
}

VOID CButtonMgr::Draw(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}

	for (int i = 0; i < m_btnVector.size(); i++)
	{
		pDriver->DrawRect(m_btnVector[i]);
		m_btnVector[i]->Draw(pDriver);
	}
}

VOID CButtonMgr::Action(CDxDriver* pDriver, CDxInput* pInput, int buttonAction)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}
	if (pInput == nullptr)
	{
		MessageBox(NULL, TEXT("No input device."), TEXT("Error"), MB_OK);
		return;
	}

	switch (buttonAction)
	{
	case 0:
		if (pDriver->ExitMessageBox() == IDYES)
			PostMessage(pInput->GetHwnd(), WM_DESTROY, 0, 0);
		break;
	case 1:
		pDriver->m_fAngle += 0.05f;
		break;
	case 2:
		pDriver->m_fAngle -= 0.05f;
		break;
	}
}