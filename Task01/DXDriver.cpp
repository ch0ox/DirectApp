/*------------------------------------------------------------------------*/
//								CDxDriver.cpp
/*------------------------------------------------------------------------*/


#include "DXDriver.h"

#include "main.h"
#include "WinApp.h"
#include "KeyboardMgr.h"
#include "MouseMgr.h"
#include "Draw.h"
#include "Button.h"
#include "ObjMgr.h"
#include "Timer.h"
#include "DXInput.h"

CDxDriver::CDxDriver(App* app)	:	m_pApp(app)
{
	m_pDraw = nullptr;
	m_pDraw = new CDraw();
	m_pDraw->LinkD3D(this);

	m_pMouse = nullptr;
	m_pMouse = new CMouse();
	m_pMouse->LinkD3D(this);
}

CDxDriver::~CDxDriver()
{
	Term();
}


BOOL CDxDriver::Initialize(HWND hWnd)												// App �� HWND �޾� ��.
{
	m_hWnd = hWnd;

	if (SUCCEEDED(InitD3D()))
	{
		if (SUCCEEDED(InitVB()))
		{
			if (SUCCEEDED(InitGeometry()))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

HRESULT CDxDriver::InitD3D()
{
	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

	// ó���� â ���� ����.
	m_d3dpp.Windowed = WindowMode;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		// SWAP �� �� ȿ����.
	m_d3dpp.hDeviceWindow = m_hWnd;

	m_d3dpp.BackBufferFormat = mode.Format;

	if (WindowMode)
	{
		m_d3dpp.BackBufferWidth = nWidth;
		m_d3dpp.BackBufferHeight = nHeight;
	}
	else											// FULL SCREEN
	{
		m_d3dpp.BackBufferWidth = mode.Width;
		m_d3dpp.BackBufferHeight = mode.Height;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
	}

	m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// HAL ǻ�� �ϵ���� ����̽� �õ�
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE
		, &m_d3dpp, &m_pD3DDevice)))
	{
		// �ϵ���� �� T&L �õ�
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
		{
			// ����Ʈ���� ����
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
			{
				return E_FAIL;
			}	// Hal - SW VP
		}	// Hal - HW VP
	}	// Hal - Pure

	return S_OK;
}


HRESULT CDxDriver::InitVB()
{
	m_pDraw->CreateTriangleBuffer();
	m_pDraw->TextInit();

	return S_OK;
}



HRESULT CDxDriver::InitGeometry()
{

	m_pDraw->CreateButton();
	m_pDraw->TriangleInit();

	for (int i = 0; i < static_cast<int>(m_pDraw->m_btnVector.size()); i++)
	{
		m_pDraw->m_btnVector[i]->CreateTexture(m_pD3DDevice);
	}

	m_pMouse->LinkButton(m_pDraw->m_btnVector);

	return S_OK;
}

VOID CDxDriver::Drawing()
{
	// Text
	m_pDraw->SetDuringTime(static_cast<float>(m_pApp->GetTimer()->GetFPS()));
	m_pDraw->DrawTextFPS();

	// Triangle
// 	m_pD3DDevice->SetTexture(0, m_pDraw->m_pTexture);
// 	m_pDraw->SetupMatrices();
// 	m_pDraw->DrawTriangle();

	// Button
	for (int i = 0; i < static_cast<int>(m_pDraw->m_btnVector.size()); i++)
	{
		m_pD3DDevice->SetTexture(0, m_pDraw->m_btnVector[i]->m_pTexture);
		m_pDraw->m_btnVector[i]->SetTexture();
		m_pDraw->DrawRect(m_pDraw->m_btnVector[i]);
	}
}

VOID CDxDriver::InputRender(CDxInput* pInput)
{
	pInput->GetDevice();

	// KeyBoard
	if (pInput->pKey->KeyDown(DIK_ESCAPE))
	{
		if (ExitMessageBox() == IDYES)
			PostMessage(pInput->GetHwnd(), WM_DESTROY, 0, 0);
		else
			return;
	}
	else if (pInput->pKey->KeyDown(DIK_SPACE))
	{
		MessageBox(NULL, TEXT("Space Key Success"), TEXT("Yeahhhh"), MB_OK);
		pInput->pKey->Toggle(this);
	}
	else if (pInput->pKey->KeyDown(DIK_LEFT))
	{

	}
	else if (pInput->pKey->KeyDown(DIK_RIGHT))
	{

	}

	// Mouse
	POINT pt;
	pt = pInput->m_pMouse->ClientCursorPos(m_hWnd);
}

BOOL CDxDriver::Render(CDxInput* pInput)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	// Timer
	m_pApp->GetTimer()->Tick(60.0f);

	if (m_bLostDevice)
	{
		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		// Resource ���� ����			 
		if (hr == D3DERR_DEVICELOST)							// Device Lost + ���� �Ұ���
		{
			return FALSE;
		}
		else if (hr == D3DERR_DEVICENOTRESET)						// Device Lost + ���� ����
		{
			// Reset
			HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);

			InitVB();
			InitGeometry();
			if (FAILED(hr))
			{
				MessageBox(NULL, TEXT("Device Reset Error"), TEXT("Device Error"), MB_OK);
			}

			m_bLostDevice = FALSE;
		}
	}

	InputRender(pInput);

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// Rendering Start 
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		Drawing();
		// TO DO : obj ������ �ִٸ� draw ���ֱ�
		// obj drawing


		m_pD3DDevice->EndScene();
	}
	if (FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))		// Rendering ������ ȭ������ ����.
	{
		// Lost
		m_bLostDevice = TRUE;
	}

	return TRUE;
}

HRESULT CDxDriver::SetTexture(UINT texture)
{
	if (texture == 0)
	{
		return m_pD3DDevice->SetTexture(0, nullptr);
	}
	return m_pD3DDevice->SetTexture(0, m_pTextureList[texture]);
}

VOID CDxDriver::Term()
{
	if (m_pMouse)
	{
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	if (m_pDraw)
	{
		delete m_pDraw;
		m_pDraw = nullptr;
	}

	for (int i = 0; i < m_pTextureList.size(); i++)
	{
		SAFE_RELEASE(m_pTextureList[i]);
	}
	m_pTextureList.clear();

	for (int i = 0; i < m_pIndexBufferList.size(); i++)
	{
		SAFE_RELEASE(m_pIndexBufferList[i]);
	}
	m_pIndexBufferList.clear();

	for (int i = 0; i < m_pVertexBufferList.size(); i++)
	{
		SAFE_RELEASE(m_pVertexBufferList[i]);
	}
	m_pVertexBufferList.clear();

	if (m_pTexture != NULL)
		SAFE_RELEASE(m_pTexture);

	if (m_pFont != NULL)
	{
		SAFE_RELEASE(m_pFont);
		m_pDesc = { NULL };
	}

	if (m_pIB != NULL)
		SAFE_RELEASE(m_pIB);

	if (m_pVB_Tri != NULL)
		SAFE_RELEASE(m_pVB_Tri);

	if (m_pVB != NULL)
		SAFE_RELEASE(m_pVB);

	if (m_pD3DDevice != NULL)
		SAFE_RELEASE(m_pD3DDevice);

	if (m_pD3D != NULL)
		SAFE_RELEASE(m_pD3D);
}


INT CDxDriver::ExitMessageBox()
{
	int result = MessageBox(NULL, (LPCWSTR)L"������ ���� �Ͻðڽ��ϱ�?", (LPCWSTR)L"INFO",
		MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);
	return result;
}


VOID CDxDriver::ChangeDisplayMode(int mode)
{
	D3DDISPLAYMODE dmode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	int monitorX = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2)) - nWidth / 2;
	int monitorY = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2)) - nHeight / 2;

	switch (mode)
	{
		RECT rect;
	case windowMode:
		SetRect(&rect, 0, 0, nWidth, nHeight);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		break;

	case fullScreenMode:
		SetRect(&rect, 0, 0, dmode.Width, dmode.Height);
		AdjustWindowRect(&rect, WS_VISIBLE | WS_POPUP, FALSE);

		SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		SetWindowPos(m_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		break;
	}
	return;
}

VOID CDxDriver::DeviceLostRecovery()
{
	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	{
		ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
		m_d3dpp.Windowed = WindowMode;
		m_d3dpp.BackBufferCount = 1;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.hDeviceWindow = m_hWnd;
		m_d3dpp.BackBufferFormat = mode.Format;
		m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		if (WindowMode)
		{
			m_d3dpp.BackBufferWidth = nWidth;
			m_d3dpp.BackBufferHeight = nHeight;
		}
		else												// FULL SCREEN
		{
			m_d3dpp.BackBufferWidth = mode.Width;
			m_d3dpp.BackBufferHeight = mode.Height;
			m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
		}

	}
}

// TO DO : Obj Model Draw
VOID CDxDriver::DrawObjModel(CObjMgr* p_ObjMgr)
{
	m_pD3DDevice->SetFVF(p_ObjMgr->GetFVF());
	m_pD3DDevice->SetStreamSource(0, m_pVertexBufferList[0] // VB
								, 0, sizeof(OBJVERTEX));
}